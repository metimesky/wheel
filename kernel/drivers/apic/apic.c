#include "apic.h"
#include <drivers/acpi/acpi.h>
#include <timming/timming.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>

int local_apic_count = 0;
int io_apic_count = 0;

// once the local APIC is enabled, 

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
        case ACPI_MADT_TYPE_LOCAL_APIC: {
            ++local_apic_count;
            log("acpi::madt LOCAL_APIC");
            // should be initialized using each AP.
            // local_apic_init(((ACPI_MADT_LOCAL_APIC *) p)->Address);
        }
            break;
        case ACPI_MADT_TYPE_IO_APIC: {
            ++io_apic_count;
            // log("acpi::madt IO_APIC");
            io_apic_init(((ACPI_MADT_IO_APIC *) p)->Address);
        }
            break;
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE: {
            ACPI_MADT_INTERRUPT_OVERRIDE *override = (ACPI_MADT_INTERRUPT_OVERRIDE *) p;
            log("acpi::madt INTERRUPT_OVERRIDE %s src:%d gbl:%d", (override->Bus==0)?"ISA":"?", override->SourceIrq, override->GlobalIrq);
        }
            break;
        case ACPI_MADT_TYPE_NMI_SOURCE: {
            ACPI_MADT_NMI_SOURCE *nmi = (ACPI_MADT_NMI_SOURCE *) p;
            log("acpi::madt NMI_SOURCE gbl: %d", nmi->GlobalIrq);
        }
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_NMI: {
            ACPI_MADT_LOCAL_APIC_NMI *nmi = (ACPI_MADT_LOCAL_APIC_NMI *) p;
            log("acpi::madt LOCAL_APIC_NMI core:%d, LINT%d", nmi->ProcessorId, nmi->Lint);
        }
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE: {
            // if this subtable present, then we must use the address provided in this table
            ACPI_MADT_LOCAL_APIC_OVERRIDE *override = (ACPI_MADT_LOCAL_APIC_OVERRIDE *) p;
            log("acpi::madt LOCAL_APIC_OVERRIDE %d", override->Address);
        }
            break;
        case ACPI_MADT_TYPE_IO_SAPIC: {
            ACPI_MADT_IO_SAPIC *iosapic = (ACPI_MADT_IO_SAPIC *) p;
            log("acpi::madt IO_SAPIC #%d addr: %x, base %d", iosapic->Id, iosapic->Address, iosapic->GlobalIrqBase);
        }
            break;
        case ACPI_MADT_TYPE_LOCAL_SAPIC: {
            ACPI_MADT_LOCAL_SAPIC *sapic = (ACPI_MADT_LOCAL_SAPIC *) p;
            log("acpi::madt LOCAL_SAPIC");
        }
            break;
        case ACPI_MADT_TYPE_INTERRUPT_SOURCE: {
            ACPI_MADT_INTERRUPT_SOURCE *src = (ACPI_MADT_INTERRUPT_SOURCE *) p;
            log("acpi::madt INTERRUPT_SOURCE");
        }
            break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC:
            log("acpi::madt LOCAL_X2APIC");
            break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI:
            log("acpi::madt LOCAL_X2APIC_NMI");
            break;
        case ACPI_MADT_TYPE_GENERIC_INTERRUPT:
            log("acpi::madt GENERIC_INTERRUPT");
            break;
        case ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR:
            log("acpi::madt GENERIC_DISTRIBUTOR");
            break;
        case ACPI_MADT_TYPE_GENERIC_MSI_FRAME:
            log("acpi::madt GENERIC_MSI_FRAME");
            break;
        case ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR:
            log("acpi::madt GENERIC_REDISTRIBUTOR");
            break;
        case ACPI_MADT_TYPE_GENERIC_TRANSLATOR:
            log("acpi::madt GENERIC_TRANSLATOR");
            break;
        case ACPI_MADT_TYPE_RESERVED:
            log("acpi::madt RESERVED");
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
