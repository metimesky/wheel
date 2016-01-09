#include "hpet.h"
#include <utilities/logging.h>

bool hpet_init() {
    ACPI_TABLE_HPET *hpet;
    ACPI_STATUS status = AcpiGetTable(ACPI_SIG_HPET, 1, &hpet);
    if (ACPI_FAILURE(status)) {
        log("Cannot locate HPET");
        return false;
    } else {
        log("located HPET at %x", hpet);
        return true;
    }
}