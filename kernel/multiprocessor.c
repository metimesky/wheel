#include <type.h>

#define _MP_ (('_'<<0)|('M'<<8)|('P'<<16)|('_'<<24))

struct mp_floating_pointer {
    uint32_t signature;
    uint32_t pointer;
    uint8_t length;
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t feature[5];
};
typedef struct mp_floating_pointer mp_floating_pointer_t;

uint64_t find_mp_floating_pointer(uint64_t ebda_addr, uint64_t base_mem_size) {
    // Notice: MP Floating Pointer Structure begins on a 16-bytes boundary.
    // However, uint32_t type spans 4 bytes, adding 4 to `sig` increases 16 bytes.
    uint32_t *sig;

    // first search the first KB of EBDA
    ebda_addr &= ~(16 - 1);     // align ebda_addr on 16-bytes boundary
    for (sig = ebda_addr; sig < ebda_addr + 0x400; sig += 4) {
        if (*sig == _MP_) {
            return sig;
        }
    }
    // then search the last KB of system base memory
    base_mem_size <<= 10;       // base mem size is reported in KB - 1K
    for (sig = base_mem_size; sig < base_mem_size + 0x400; sig += 4) {
        if (*sig == _MP_) {
            return sig;
        }
    }
    // search BIOS ROM between 0xe0000 and 0xfffff;
    for (sig = 0xe0000; sig < 0x100000; sig += 4) {
        if (*sig == _MP_) {
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
    mp_floating_pointer_t *pointer = (mp_floating_pointer_t *)
        find_mp_floating_pointer(ebda_addr, base_mem_size);
    if (pointer) {
        print("MP pointer located", 800);
        uint8_t sum = 0;
        // length are measured in 16 bytes
        for (uint8_t *i = pointer; i < (uint64_t) pointer + pointer->length * 16; ++i) {
            sum += *i;
        }
        if (sum == 0) {
            print("and checksum passed", 880);
        } else {
            print("but checksum failed", 880);
        }
    } else {
        print("MP pointer not found!", 800);
    }
}