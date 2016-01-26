#include "apic.h"
#include <drivers/acpi/acpi.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>
#include <drivers/pit/pit.h>
#include <scheduler/scheduler.h>

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
    ent2 |= (((uint64_t) local_apic_arr[0]->Id) << 56) & 0xff00000000000000UL;    // target
    pit_map_gsi(80);
    io_apic_set_gsi(2, ent2);
    io_apic_unmask(2);

    // after, init BSP's local APIC
    local_apic_local_init();

    // after local APIC timer is initialized, we can disable PIT
    // io_apic_mask(2);

    return true;
}
