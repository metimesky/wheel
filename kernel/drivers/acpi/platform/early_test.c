#include "../acpi.h"
#include <utilities/clib.h>
#include <utilities/logging.h>

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

// static allocated space
#define TABLE_DESC_NUM 16
static ACPI_TABLE_DESC table_descs[TABLE_DESC_NUM];

// early ACPI table access, without dynamic memory support
void initialize_acpi_tables() {
    ACPI_STATUS status;

    // first init table management component
    // status = AcpiInitializeTables(table_descs, TABLE_DESC_NUM, FALSE);
    status = AcpiInitializeTables(NULL, 16, FALSE);
    if (ACPI_FAILURE(status)) {
        log("acpica::table early init failed, err code %u", status);
    }

    // then find tables one by one
    ACPI_TABLE_MADT *madt;
    status = AcpiGetTable(ACPI_SIG_MADT, 1, &madt);
    if (ACPI_FAILURE(status)) {
        log("Cannot locate MADT");
    } else {
        log("located MADT at %x", madt);
    }

    ACPI_TABLE_HPET *hpet;
    status = AcpiGetTable(ACPI_SIG_HPET, 1, &hpet);
    if (ACPI_FAILURE(status)) {
        log("Cannot locate HPET");
    } else {
        log("located HPET at %x", hpet);
    }
}

// full ACPI initialization
void initialize_full_acpi() {
    ACPI_STATUS status;

    // initialize the ACPICA subsystem
    status = AcpiInitializeSubsystem();
    if (ACPI_FAILURE(status)) {
        log("acpica subsystem init failed, err code %u", status);
        return;
    }

    // initialize the ACPICA Table Manager and get all ACPI tables
    status = AcpiInitializeTables(NULL, 16, FALSE);
    if (ACPI_FAILURE(status)) {
        log("acpica table init failed, err code %u", status);
        return;
    }
}

// test ACPICA table management component
void early_test() {
    ACPI_STATUS status;

    // status = AcpiInitializeSubsystem();
    // if (ACPI_FAILURE(status)) {
    //     log("cannot init apic subsystem");
    //     return;
    // }

    // early initialize table management component
    // #define DESC_NUM 2
    ACPI_TABLE_DESC tables[16];
    log("size of DESC is %d", sizeof(ACPI_TABLE_DESC));
    // status = AcpiInitializeTables(tables, 8, TRUE);
    status = AcpiInitializeTables(NULL, 16, FALSE);
    if (ACPI_FAILURE(status)) {
        log("acpica::tbl init failed with err code %u", status);
    } else {
        log("acpica::tbl init success!");
    }

    for (int i = 0; i < 2; ++i) {
        if (strncmp(tables[i].Signature.Ascii, ACPI_SIG_MADT, 4) == 0) {
            ACPI_TABLE_MADT *madt = (ACPI_TABLE_MADT *) tables[i].Address;
            log("found madt at %x, header %x, len %d", madt, tables[i].Pointer, tables[i].Length);
            process_madt(madt);
        } else if (strncmp(tables[i].Signature.Ascii, ACPI_SIG_HPET, 4) == 0) {
            log("found hpet");
        } else if (strncmp(tables[i].Signature.Ascii, ACPI_SIG_BOOT, 4) == 0) {
            log("found boot");
        } else if (strncmp(tables[i].Signature.Ascii, ACPI_SIG_UEFI, 4) == 0) {
            log("found uefi");
        }
    }

    return;

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