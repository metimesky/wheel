#include <type.h>
#include "multiprocessor.h"
#include "fake_console.h"

static int is_mp_pointer_valid(mp_pointer_t *pointer) {
    uint8_t sum = 0;
    // length are measured in 16 bytes
    for (uint8_t *i = pointer; i < (uint64_t) pointer + pointer->length * 16; ++i) {
        sum += *i;
    }
    return sum == 0;
}

static int is_mp_conf_valid(mp_conf_t *conf) {
    if (conf->signature != PCMP) {
        println("PCMP signature not match!");
        return 0;
    }
    uint8_t sum = 0;
    for (uint8_t *i = conf; i < (uint64_t) conf + conf->base_length; ++i) {
        sum += *i;
    }
    return sum == 0;
}

// return the address of MP Floating Pointer Structure, or 0 if not found
// MP Floating Pointer Structure is in one of three memory areas:
// 1. first KB of EBDA
// 2. the last KB of base memory (639-640KB)
// 3. the BIOS ROM address space (0xf0000-0xfffff)
uint64_t find_mp_pointer() {
    // Notice: MP Floating Pointer Structure begins on a 16-bytes boundary.
    // However, uint32_t type spans 4 bytes, adding 4 to `sig` increases 16 bytes.
    uint32_t *sig;

    // get base memory size from word in [40:13] of the BIOS data area
    uint64_t base_mem_size = *((uint16_t *) 0x0413U);
    // and EBDA starting address from [40:0e] of BIOS data area
    uint64_t ebda_addr = *((uint16_t *) 0x040eU);

    // first search the first KB of EBDA
    ebda_addr &= ~(16 - 1);     // align ebda_addr on 16-bytes boundary
    for (sig = ebda_addr; sig < ebda_addr + 0x400; sig += 4) {
        if (*sig == _MP_ && is_mp_pointer_valid(sig)) {
            return sig;
        }
    }
    // then search the last KB of system base memory
    base_mem_size <<= 10;       // base mem size is reported in KB - 1K
    for (sig = base_mem_size; sig < base_mem_size + 0x400; sig += 4) {
        if (*sig == _MP_ && is_mp_pointer_valid(sig)) {
            return sig;
        }
    }
    // search BIOS ROM between 0xe0000 and 0xfffff;
    for (sig = 0xe0000; sig < 0x100000; sig += 4) {
        if (*sig == _MP_ && is_mp_pointer_valid(sig)) {
            return sig;
        }
    }
    return 0;
}

// return 0 on multi-processor, 1 on uni-processor
int multiprocessor_init() {
    // informations that need to be gathered
    uint64_t local_apic_addr = 0;
    uint64_t io_apic_num = 0;
    uint64_t io_apic_ids[32];   // up to 32 io apic are supported
    uint64_t io_apic_addrs[32];

    // 1. find MP Floating Pointer Structure
    mp_pointer_t *pointer = (mp_pointer_t *) find_mp_pointer();
    if (!pointer) {
        println("MP pointer not found!");
        return 1;
    }

    // 2. locate MP Configuration Table
    mp_conf_t *conf = (mp_conf_t *) pointer->pointer;
    if (!(conf && is_mp_conf_valid(conf))) {
        print("MP configure table not valid, fallback to default configurations.");
        // default configuration means that:
        // 1. system has two processors
        // 2. local APICs map to 0xfee00000, IDs start from 0
        // 3. system has one IO APIC which maps to 0xfec00000
        println("But that's not implemented yet");
        local_apic_addr = 0xfee00000UL;
        return 1;
    } else {
        char oem_id[9], product_id[13];
        for (int i = 0; i < 8; ++i) {
            oem_id[i] = conf->oem_id[i];
        }
        oem_id[8] = '\0';
        for (int i = 0; i < 12; ++i) {
            product_id[i] = conf->product_id[i];
        }
        product_id[12] = '\0';
        print("Oem id: ");
        print(oem_id);
        print(", product id: ");
        println(product_id);

        local_apic_addr = conf->local_apic_addr;

        uint8_t *entry = conf->entries;
        char buf[33];
        print("looping over ");
        print(u32_to_str(conf->entry_count, buf, 10));
        print(" entries:");
        uint8_t last_type = -1;
        for (int i = 0; i < conf->entry_count; ++i) {
            switch (*entry) {
            case 0: {   // processor
                if (last_type != 0) {
                    last_type = 0;
                    println("");
                    print("# processor [local APIC id]> ");
                }
                mp_processor_entry_t *e = (mp_processor_entry_t *) entry;
                print(u32_to_str(e->local_apic_id, buf, 10));
                print(", ");
                entry += sizeof(mp_processor_entry_t);
            }
                break;
            case 1: {   // bus
                if (last_type != 1) {
                    last_type = 1;
                    println("");
                    print("# bus [id]> ");
                }
                mp_bus_entry_t *e = (mp_bus_entry_t *) entry;
                print(u32_to_str(e->bus_id, buf, 10));
                print(", ");
                entry += sizeof(mp_bus_entry_t);
            }
                break;
            case 2: {   // io apic
                if (last_type != 2) {
                    last_type = 2;
                    println("");
                    print("# IO APIC [id]> ");
                }
                mp_io_apic_entry_t *e = (mp_io_apic_entry_t *) entry;
                io_apic_ids[io_apic_num] = e->io_apic_id;
                io_apic_addrs[io_apic_num] = e->address;
                ++io_apic_num;
                print(u32_to_str(e->io_apic_id, buf, 10));
                print(", ");
                entry += sizeof(mp_io_apic_entry_t);
            }
                break;
            case 3: {   // io interrupt assignment
                if (last_type != 3) {
                    last_type = 3;
                    println("");
                    print("# IO int assign [src bus id:irq --> dst IO APIC id:intin]> ");
                }
                mp_io_interrupt_entry_t *e = (mp_io_interrupt_entry_t *) entry;
                print("(");
                print(u32_to_str(e->src_bus_id, buf, 10));
                print(":");
                print(u32_to_str(e->src_bus_irq, buf, 10));
                print(") --> (");
                print(u32_to_str(e->dst_io_apic_id, buf, 10));
                print(":");
                print(u32_to_str(e->dst_io_apic_intin, buf, 10));
                print("), ");
                entry += sizeof(mp_io_interrupt_entry_t);
            }
                break;
            case 4: {   // local interrupt assignment
                if (last_type != 4) {
                    last_type = 4;
                    println("");
                    print("# local int assign [src bus id:irq --> dst local APIC id:lintin]> ");
                }
                mp_local_interrupt_entry_t *e = (mp_local_interrupt_entry_t *) entry;
                print("(");
                print(u32_to_str(e->src_bus_id, buf, 10));
                print(":");
                print(u32_to_str(e->src_bus_irq, buf, 10));
                print(") --> (");
                print(u32_to_str(e->dst_local_apic_id, buf, 10));
                print(":");
                print(u32_to_str(e->dst_local_apic_lintin, buf, 10));
                print("), ");
                entry += sizeof(mp_local_interrupt_entry_t);
            }
                break;
            case 128:   // system address space mapping
                println("-- system address space mapping");
                entry += 20;
                break;
            case 129:   // bus hierarchy descriptor
                println("-- system address space mapping");
                entry += 8;
                break;
            case 130:   // compatibility bus address space modifier
                println("-- system address space mapping");
                entry += 8;
                break;
            default:
                println("-- WTF!!!");
                break;
            }
        }
        println("all done");
    }

    // 3. initialize local APIC of BSP
    return 0;
}