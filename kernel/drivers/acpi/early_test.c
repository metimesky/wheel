#include "acpica/acpi.h"
#include <utilities/logging.h>

// test ACPICA table management component
void early_test() {
    ACPI_STATUS status;

    // status = AcpiInitializeSubsystem();
    // if (ACPI_FAILURE(status)) {
    //     log("cannot init apic subsystem");
    //     return;
    // }

    // early initialize table management component
    #define DESC_NUM 16
    ACPI_TABLE_DESC tables[DESC_NUM];
    status = AcpiInitializeTables(tables, DESC_NUM, FALSE);
    if (ACPI_FAILURE(status)) {
        log("acpica::tbl init failed with err code %u", status);
    } else {
        log("acpica::tbl init success!");
    }

    // initialize ACPICA subsystems
    status = AcpiInitializeSubsystem();
    if (ACPI_FAILURE(status)) {
        log("acpica init failed");
    }

    status = AcpiLoadTables();
    if (ACPI_FAILURE(status)) {
        log("acpica::tbl load tables failed with err code %u", status);
    } else {
        log("acpica::tbl load tables success!");
    }
}