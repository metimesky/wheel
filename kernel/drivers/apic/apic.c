#include "apic.h"
#include <drivers/acpi/acpi.h>
#include <timming/timming.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>

int local_apic_count = 0;
int io_apic_count = 0;

// generic APIC initialization routine
// return false when APIC is not available.
bool apic_init() {
    ACPI_TABLE_MADT *madt = NULL;

    // acquire APIC info from MADT
    if (ACPI_FAILURE(AcpiGetTable(ACPI_SIG_MADT, 1, &madt))) {
        log("Cannot locate MADT");
        // TODO: MADT not found, so we should find MP tables instead.
        // what does it mean, it means we have to stick with dual 8259 PICs
        return false;
    }

    local_apic_count = 0;
    io_apic_count = 0;

    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT);
    while (p < end) {
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_LOCAL_APIC:
            ++local_apic_count;
            // process_local_apic((ACPI_MADT_LOCAL_APIC *) p);
            log("ACPI_MADT_TYPE_LOCAL_APIC");
            break;
        case ACPI_MADT_TYPE_IO_APIC:
            ++io_apic_count;
            // process_io_apic((ACPI_MADT_IO_APIC *) p);
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

    log("total %d ioapic and %d lapic", io_apic_count, local_apic_count);

    // local_apic_init(madt->Address);

    // finally, configure BSP's local APIC
    local_apic_init(madt->Address);
    // bsp_local_apic_id = ;

    return true;
}
