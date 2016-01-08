#include "apic.h"
#include <drivers/acpi/acpi.h>
#include <timming/timming.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>

// initialize APIC with information provided by ACPI
void apic_init_with_madt(ACPI_TABLE_MADT *madt) {
    ;
}

// initialize APIC with information from MP tables
void apic_init_with_mp_table() {
    ;
}

int local_apic_count = 0;
int io_apic_count = 0;

// generic APIC initialization routine
void apic_init() {
    ACPI_TABLE_MADT *madt = NULL;
    if (ACPI_FAILURE(AcpiGetTable(ACPI_SIG_MADT, 1, &madt))) {
        log("Cannot locate MADT, I don't use MP Tables!");
        // TODO: should use MP table as fallback
        return;
    }
    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT);
    while (p < end) {
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_LOCAL_APIC: {
            ACPI_MADT_LOCAL_APIC *local_apic = (ACPI_MADT_LOCAL_APIC *) p;
            log("ACPI_MADT_TYPE_LOCAL_APIC");
        }
            break;
        case ACPI_MADT_TYPE_IO_APIC:
            log("ACPI_MADT_TYPE_IO_APIC");
            break;
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
            log("ACPI_MADT_TYPE_INTERRUPT_OVERRIDE");
            break;
        case ACPI_MADT_TYPE_NMI_SOURCE:
            log("ACPI_MADT_TYPE_NMI_SOURCE");
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_NMI:
            log("ACPI_MADT_TYPE_LOCAL_APIC_NMI");
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
            log("ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE");
            break;
        case ACPI_MADT_TYPE_IO_SAPIC:
            log("ACPI_MADT_TYPE_IO_SAPIC");
            break;
        case ACPI_MADT_TYPE_LOCAL_SAPIC:
            log("ACPI_MADT_TYPE_LOCAL_SAPIC");
            break;
        case ACPI_MADT_TYPE_INTERRUPT_SOURCE:
            log("ACPI_MADT_TYPE_INTERRUPT_SOURCE");
            break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC:
            log("ACPI_MADT_TYPE_LOCAL_X2APIC");
            break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI:
            log("ACPI_MADT_TYPE_LOCAL_X2APIC_NMI");
            break;
        case ACPI_MADT_TYPE_GENERIC_INTERRUPT:
            log("ACPI_MADT_TYPE_GENERIC_INTERRUPT");
            break;
        case ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR:
            log("ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR");
            break;
        case ACPI_MADT_TYPE_GENERIC_MSI_FRAME:
            log("ACPI_MADT_TYPE_GENERIC_MSI_FRAME");
            break;
        case ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR:
            log("ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR");
            break;
        case ACPI_MADT_TYPE_GENERIC_TRANSLATOR:
            log("ACPI_MADT_TYPE_GENERIC_TRANSLATOR");
            break;
        case ACPI_MADT_TYPE_RESERVED:
            log("ACPI_MADT_TYPE_RESERVED");
            break;
        default:
            break;
        }
        p += ((ACPI_SUBTABLE_HEADER *) p)->Length;
    }
}

void io_apic_init(uint64_t base) {
    ;
}
/*
void local_apic_init(uint64_t base) {
    // set the physical address for local APIC registers
    uint64_t msr_apic_base = (uint64_t) base & (~0xfffUL);
    msr_apic_base |= 1UL << 8;  // this processor is BSP
    write_msr(0x1b, msr_apic_base);

    // enable local apic
    uint32_t spurious = * ((uint64_t *) base + SPURIOUS_INT_VECTOR);
    * ((uint64_t *) base + SPURIOUS_INT_VECTOR) = spurious | 0x100;

    // local APIC timer setup

    // block if a timer event is pending
    while (DATA_U32(base + LVT_TIMER) & (1U << 12)) {}

    // enable timer
    DATA_U32(base + LVT_TIMER) = 1UL << 16;

    DATA_U32(base + DIVIDE_CONFIGURATION) = 0;   // divide by 2
    DATA_U32(base + INITIAL_COUNT) = 0xffffffff; // maximum countdown

    uint64_t tick_a = tick;
    uint32_t count_a = DATA_U32(base + CURRENT_COUNT);

    while (tick < tick_a + 50) {}
    uint32_t count_b = DATA_U32(base + CURRENT_COUNT);

    // disable timer
    DATA_U32(base + LVT_TIMER) = 0UL;

    log("tick 50's differiential is %d.", count_a - count_b);
}
*/