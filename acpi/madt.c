#include <stdhdr.h>
#include "madt.h"

extern int is_sdt_valid(sdt_header_t *sdt);

void process_madt(madt_t *madt) {
    if (!is_sdt_valid(madt)) {
        println("ACPI::MADT not valid!");
        return;
    }
    print("apic local controller addr is 0x");
    char buf[33];
    print(u32_to_str(madt->local_controller_addr, buf, 16));
    if (madt->flags == 1) {
        println(" with dual 8259 legacy.");
    } else {
        println(" without dual 8259 legacy.");
    }
    // return;
    uint8_t *p = madt->entries;
    // int i = 0;
    print("header len is ");
    println(u32_to_str(madt->header.length, buf, 10));
    while (p < (uint8_t *) madt + madt->header.length/* && i < 30*/) {
        switch (*p) {
        case 0: {   // processor local APIC
            madt_local_apic_entry_t *entry = (madt_local_apic_entry_t *) p;
            print("processorId ");
            print(u32_to_str(entry->processor_id, buf, 10));
            print(" APICId ");
            print(u32_to_str(entry->local_apic_id, buf, 10));
            print(" flag ");
            println(u32_to_str(entry->flags, buf, 10));
            p += entry->length;
        }
            break;
        case 1: {   // IO APIC
            madt_io_apic_entry_t *entry = (madt_io_apic_entry_t *) p;
            print("IO APICId=");
            print(u32_to_str(entry->io_apic_id, buf, 10));
            print(" addr=0x");
            print(u32_to_str(entry->io_apic_addr, buf, 16));
            print(" int_base=0x");
            println(u32_to_str(entry->global_system_interrupt_base, buf, 16));
            p += entry->length;
        }
            break;
        case 2: {   // interrupt source override
            madt_interrupt_src_override_t *entry = (madt_interrupt_src_override_t *) p;
            // println("interrupt source override");
            print("int from bus ");
            print(u32_to_str(entry->bus_source, buf, 10));
            print(" irq=0x");
            print(u32_to_str(entry->irq_source, buf, 16));
            print(" glocal=0x");
            println(u32_to_str(entry->global_system_interrupt, buf, 16));
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
