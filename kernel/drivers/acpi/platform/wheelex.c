// we should give this file another name.

#include "../acpi.h"
#include <utilities/clib.h>
#include <utilities/logging.h>

static ACPI_TABLE_DESC acpi_tables[16];

// early ACPI table access, without dynamic memory support
bool initialize_acpi_tables() {
    // ACPI_STATUS status = AcpiInitializeTables(NULL, 16, FALSE);
    ACPI_STATUS status = AcpiInitializeTables(acpi_tables, 16, FALSE);
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
