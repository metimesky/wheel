#include <utilities/clib.h>
#include <utilities/logging.h>
#include "madt.h"

bool madt_present = false;

int io_apic_num = 0;
uint32_t io_apic_base[12];
uint32_t local_apic_base;

int processor_num = 0;
uint8_t processor_ids[128];
uint8_t local_apic_ids[128];

void process_madt(madt_t *madt) {
    if (!is_sdt_valid(madt)) {
        log("ACPI::MADT not valid!");
        return;
    }
    madt_present = true;
    // log("apic local controller addr is 0x%x.", madt->local_controller_addr);
    local_apic_base = madt->local_controller_addr;

    if (madt->flags == 1) {
        // log("has dual 8259 legacy.");
    } else {
        // log("no dual 8259 legacy.");
    }

    uint8_t *p = madt->entries;
    // log("header len is %d", madt->header.length);
    while (p < (uint8_t *) madt + madt->header.length) {
        switch (*p) {
        case 0: {   // processor local APIC
            madt_local_apic_entry_t *entry = (madt_local_apic_entry_t *) p;
            processor_ids[processor_num] = entry->processor_id;
            log("processorId %d APICId %d flag %d", entry->processor_id, entry->local_apic_id, entry->flags);
            p += entry->length;
        }
            break;
        case 1: {   // IO APIC
            madt_io_apic_entry_t *entry = (madt_io_apic_entry_t *) p;
            io_apic_base[io_apic_num] = entry->io_apic_addr;
            ++io_apic_num;
            log("IO APICId=%d addr=0x%x int_base=0x%x", entry->io_apic_id, entry->io_apic_addr, entry->global_system_interrupt_base);
            p += entry->length;
        }
            break;
        case 2: {   // interrupt source override
            madt_interrupt_source_override_t *entry = (madt_interrupt_source_override_t *) p;
            log("int from bus %d irq=0x%x glocal=0x%x", entry->bus, entry->source, entry->global_system_interrupt);
            p += entry->length;
        }
            break;
        default:
            // although we don't know the structure of this entry
            // but we're sure that the 2nd byte indicates its length
            p += *(p+1);
            break;
        }
    }
}
