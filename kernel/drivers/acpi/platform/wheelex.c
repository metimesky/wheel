// we should give this file another name.

#include "../acpi.h"
#include <utilities/clib.h>
#include <utilities/logging.h>

/*
int io_apic_count;
int local_apic_count;

static void process_madt(ACPI_TABLE_MADT *madt) {
    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    for (uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT); p < end; p += ((ACPI_SUBTABLE_HEADER *) p)->Length) {
    // while (p < end) {
        // ACPI_SUBTABLE_HEADER *sub = (ACPI_SUBTABLE_HEADER *) p;
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_LOCAL_APIC: {
            log("ACPI_MADT_TYPE_LOCAL_APIC");
            ACPI_MADT_LOCAL_APIC *local_apic = (ACPI_MADT_LOCAL_APIC *) p;
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
    }
}
*/

// early ACPI table access, without dynamic memory support
bool initialize_acpi_tables() {
    ACPI_STATUS status = AcpiInitializeTables(NULL, 16, FALSE);
    if (ACPI_FAILURE(status)) {
        // Wheel requires ACPI to be available.
        return false;
    }
    return true;
}

// full ACPI initialization
bool initialize_full_acpi() {
    ACPI_STATUS status;

    // initialize the ACPICA subsystem
    status = AcpiInitializeSubsystem();
    if (ACPI_FAILURE(status)) {
        log("acpica subsystem init failed, err code %u", status);
        return false;
    }

    /* if table has been initialized early, then we won't need to enable it again.
    // initialize the ACPICA Table Manager and get all ACPI tables
    status = AcpiInitializeTables(NULL, 16, FALSE);
    if (ACPI_FAILURE(status)) {
        log("acpica table init failed, err code %u", status);
        return;
    }
    */
}
