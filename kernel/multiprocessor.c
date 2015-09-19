#include <type.h>
#include "multiprocessor.h"

static int is_mp_pointer_valid(mp_pointer_t *pointer) {
    uint8_t sum = 0;
    // length are measured in 16 bytes
    for (uint8_t *i = pointer; i < (uint64_t) pointer + pointer->length * 16; ++i) {
        sum += *i;
    }
    return sum == 0;
}

static int is_mp_conf_valid(mp_conf_t *conf) {
    uint8_t sum = 0;
    for (uint8_t *i = conf; i < (uint64_t) conf + conf->base_length; ++i) {
        sum += *i;
    }
    return sum == 0;
}

// return the address of MP Floating Pointer Structure, or 0 if not found
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

extern void print(const char *str, int pos);

// return 0 on multi-processor, 1 on uni-processor
int multiprocessor_init() {
    // to detect multiprocessor, we have to find MP Floating Pointer Structure,
    // which is in one of three memory areas:
    // 1. first KB of EBDA
    // 2. the last KB of base memory (639-640KB)
    // 3. the BIOS ROM address space (0xf0000-0xfffff)

    // first get base memory size from word in [40:13h] of the BIOS data area
    uint16_t base_mem_size = *((uint16_t *) 0x0413U);
    uint16_t ebda_addr = *((uint16_t *) 0x040eU);
    mp_pointer_t *pointer = (mp_pointer_t *) find_mp_pointer(ebda_addr, base_mem_size);
    if (!pointer) {
        print("MP pointer not found!", 800);
        return 1;
    }
    print("MP pointer located", 800);
    mp_conf_t *conf = (mp_conf_t *) pointer->pointer;
    if (conf && is_mp_conf_valid(conf)) {
        print("MP configure table is present.", 880);
        char oem_id[9], product_id[13];
        for (int i = 0; i < 8; ++i) {
            oem_id[i] = conf->oem_id[i];
        }
        oem_id[8] = '\0';
        for (int i = 0; i < 12; ++i) {
            product_id[i] = conf->product_id[i];
        }
        product_id[12] = '\0';
        print(oem_id, 960);
        print(product_id, 970);
    } else {
        print("MP configure table not valid, fallback to default configurations.", 880);
        // default configuration means that:
        // 1. system has two processors
        // 2. local APICs map to 0xfee00000, IDs start from 0
        // 3. system has one IO APIC which maps to 0xfec00000
    }

    return 0;
}