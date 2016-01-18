#include "hpet.h"
#include <utilities/logging.h>

/* HPET (High Precision Event Timer) is designed to replace 8254 PIT and RTC.
 * unit of timing is femptoseconds (10^-5 seconds)
 */

/*
typedef struct acpi_table_hpet {
    ACPI_TABLE_HEADER       Header;             // Common ACPI table header
    UINT32                  Id;                 // Hardware ID of event timer block
    ACPI_GENERIC_ADDRESS    Address;            // Address of event timer block
    UINT8                   Sequence;           // HPET sequence number
    UINT16                  MinimumTick;        // Main counter min tick, periodic mode
    UINT8                   Flags;
} ACPI_TABLE_HPET;

typedef struct acpi_generic_address {
    UINT8                   SpaceId;                // Address space where struct or register exists
    UINT8                   BitWidth;               // Size in bits of given register
    UINT8                   BitOffset;              // Bit offset within the register
    UINT8                   AccessWidth;            // Minimum Access size (ACPI 3.0)
    UINT64                  Address;                // 64-bit address of struct or register
} ACPI_GENERIC_ADDRESS;
*/

bool hpet_init() {
    ACPI_TABLE_HPET *hpet;
    ACPI_STATUS status = AcpiGetTable(ACPI_SIG_HPET, 1, &hpet);
    if (ACPI_FAILURE(status)) {
        log("Cannot locate HPET");
        return false;
    } else {
        log("HPET #%d mapping at %x", hpet->Id, hpet->Address.Address);
        // return true;
        return false;
    }
}