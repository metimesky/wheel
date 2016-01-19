#include "apic.h"
#include <drivers/acpi/acpi.h>
// #include <timing/timing.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>
#include <drivers/pit/pit.h>
#include <timing/timing.h>

// generic APIC initialization routine
// return false when APIC is not available.
bool apic_init() {
    ACPI_TABLE_MADT *madt = NULL;

    // acquire APIC info from MADT
    if (ACPI_FAILURE(AcpiGetTable(ACPI_SIG_MADT, 1, &madt))) {
        log("Cannot locate MADT");
        // Wheel does not use MP tables method, so MADT's not exist means fail        
        return false;
    }

    // before parsing entries, we initialize io apic and local apic module.
    io_apic_init();
    local_apic_init(madt);

    // start parsing entries
    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT);
    while (p < end) {
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_IO_APIC:
            io_apic_add((ACPI_MADT_IO_APIC *) p);
            break;
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
            io_apic_interrupt_override((ACPI_MADT_INTERRUPT_OVERRIDE *) p);
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC:
            local_apic_add((ACPI_MADT_LOCAL_APIC *) p);
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
            local_apic_address_override((ACPI_MADT_LOCAL_APIC_OVERRIDE *) p);
            break;
        case ACPI_MADT_TYPE_NMI_SOURCE:             break;
        case ACPI_MADT_TYPE_LOCAL_APIC_NMI:         break;
        case ACPI_MADT_TYPE_IO_SAPIC:               break;
        case ACPI_MADT_TYPE_LOCAL_SAPIC:            break;
        case ACPI_MADT_TYPE_INTERRUPT_SOURCE:       break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC:           break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI:       break;
        case ACPI_MADT_TYPE_GENERIC_INTERRUPT:      break;
        case ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR:    break;
        case ACPI_MADT_TYPE_GENERIC_MSI_FRAME:      break;
        case ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR:  break;
        case ACPI_MADT_TYPE_GENERIC_TRANSLATOR:     break;
        case ACPI_MADT_TYPE_RESERVED:               break;
        default:    break;
        }
        p += ((ACPI_SUBTABLE_HEADER *) p)->Length;
    }

    // TODO: we need to initialize all ISA interrupts.

    // init GSI2, which is the PIT clock
    uint64_t ent2 = 0UL;
    ent2 |= 80UL & 0x00000000000000ffUL;    // vector = 80
    ent2 |= 0UL  & 0x0000000000000700UL;    // delivery mode = fixed
    ent2 |= 0UL << 11;                      // destination mode = physical mode
    ent2 |= 0UL << 13;                      // interrupt input pin polarity = high active
    ent2 |= 0UL << 15;                      // trigger mode = edge sensitive
    ent2 |= 1UL << 16;                      // interrupt mask
    ent2 |= ((uint64_t) local_apic_get_id(0) << 56) & 0xff00000000000000UL;    // target
    pit_map_gsi(80);
    io_apic_set_gsi(2, ent2);
    io_apic_unmask(2);

    // after, init BSP's local APIC
    local_apic_local_init();

    // after local APIC timer is initialized, we can disable PIT
    // io_apic_mask(2);

    return true;
}

static core_init(uint8_t id) {
/*    uint32_t icr_l = 0UL;
    icr |= 0U & 0x000000ffU;    // vector number
    icr |= (5U << 8) & 0x00000700UL;  // delivery mode = INIT
    icr |= 0U << 11;   // destination mode = physical
    icr |= 1U << 14;   // assert, only INIT-deAssert must be de-assert
    icr |= 1U << 15;   // trigger mode = level, only valid for INIT-deAssert
    // icr |= (0UL << 18) & 0x000c0000UL;  // destination shorthand = no
    uint32_t icr_h = 0x01000000U;  // destination field (target local apic id in physical mode)
    DATA_U32(local_apic_base + 0x310) = icr_h;
    DATA_U32(local_apic_base + 0x300) = icr_l;

    // wait for 10ms
    busy_wait(10);

    // wait for delivery
    while (DATA_U32(local_apic_base + 0x300) & (1U << 12)) {}

    log("1");
    local_apic_send_ipi(icr);   // send INIT IPI (assert)
    icr &= ~(1UL << 14);

    log("2");
    local_apic_send_ipi(icr);   // send INIT IPI (de-assert)
    busy_wait(20);

    icr = 0UL;
    icr |= 0x7cUL & 0x00000000000000ffUL;    // vector number, real mode address 0x7c000
    icr |= (6UL << 8) & 0x0000000000000700UL;  // delivery mode = StartUp
    icr |= 0UL << 11;   // destination mode = physical
    icr |= 1UL << 14;   // assert, only INIT-deAssert must be de-assert
    icr |= 0UL << 15;   // trigger mode = edge, only valid for INIT-deAssert
    icr |= (0UL << 18) & 0x00000000000c0000UL;  // destination shorthand = no
    icr |= (0UL << 56) & 0xff00000000000000UL;  // destination field (target local apic id in physical mode)

    // send STARTUP IPT
    log("3");
    local_apic_send_ipi(icr);
    busy_wait(2);

    // send STARTUP IPT
    log("4");
    local_apic_send_ipi(icr);
    busy_wait(2);

    log("5");*/
}

extern char ap;
extern char ap_end;

// initialize multiprocessor
bool apic_mp_init() {
    // set shutdown code
    out_byte(0x70, 0x0f);
    out_byte(0x71, 0x0a);

    // set warm reset vector to 0x7c000
    * ((uint16_t *) 0x0467 + 0) = 0x7000;       // segment
    * ((uint16_t *) 0x0467 + 2) = 0xc000;       // offset

    // copy real mode startup code to 0x7c000
    log("copying");
    int n = &ap_end - &ap;
    char *src = &ap;
    char *dst = (char *) 0x7c000;
    for (int i = 0; i < n; ++i) {
        dst[i] = src[i];
    }

    log("AP id is %d", local_apic_get_id(1));
    core_init(local_apic_get_id(1));

    // uint32_t icr_h = 0x01000000;
    // DATA_U32(local_apic_base + 0x310) = icr_h;



    return false;
}