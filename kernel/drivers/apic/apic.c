#include "apic.h"
#include <drivers/acpi/acpi.h>
#include <timming/timming.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>

// hard code: maximum Core/IO APIC supported
#define MAX_LOCAL_APIC_NUM 256
#define MAX_IO_APIC_NUM 16

// mapped address of local APIC
static uint64_t local_apic_addr;

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

    // retrieve base address of local APIC
    local_apic_addr = madt->Address;

    if (madt->Flags & 1) {
        // the system also has a PC-AT-compatible dual-8259 setup,
        // has to disable it properly
    }

    int local_apic_count = 0;
    int io_apic_count = 0;
    int override_count = 0;

    ACPI_MADT_LOCAL_APIC* local_apic[MAX_LOCAL_APIC_NUM];
    ACPI_MADT_IO_APIC* io_apic[MAX_IO_APIC_NUM];
    ACPI_MADT_INTERRUPT_OVERRIDE* override[16];

    // start parsing entries
    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT);
    while (p < end) {
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
            // if this subtable present, then we must use the address provided in this table
            local_apic_addr = ((ACPI_MADT_LOCAL_APIC_OVERRIDE *) p)->Address;
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC:
            local_apic[local_apic_count] = (ACPI_MADT_LOCAL_APIC *) p;
            ++local_apic_count;
            break;
        case ACPI_MADT_TYPE_IO_APIC:
            io_apic[io_apic_count] = (ACPI_MADT_IO_APIC *) p;
            ++io_apic_count;
            break;
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
            // by default, IRQ[0:15] are mapped to GSI[0:15], unless overriden by this entry
            override[override_count] = (ACPI_MADT_INTERRUPT_OVERRIDE *) p;
            ++override_count;
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

    // init IO APIC with IRQ override, currently only one IO APIC is supported
    io_apic_init(io_apic[0], override, override_count);

    log("total %d ioapic and %d lapic and %d entries", io_apic_count, local_apic_count, override_count);

    // finally, configure BSP's local APIC
    local_apic_init(madt->Address);
    // bsp_local_apic_id = ;

    return true;
}
