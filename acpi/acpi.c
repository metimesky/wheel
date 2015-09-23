#include "acpi.h"
#include <type.h>
#include <util.h>
#include "../kernel/fake_console.h"

uint64_t find_rsdp() {
    uint64_t ebda_addr = *((uint16_t *) 0x40e) << 4;

    uint64_t sig = 0UL;
    // sig |= ((uint64_t) 'R' <<  0) & 0x00000000000000ff;
    // sig |= ((uint64_t) 'S' <<  8) & 0x000000000000ff00;
    // sig |= ((uint64_t) 'D' << 16) & 0x0000000000ff0000;
    // sig |= ((uint64_t) ' ' << 24) & 0x00000000ff000000;
    // sig |= ((uint64_t) 'P' << 32) & 0x000000ff00000000;
    // sig |= ((uint64_t) 'T' << 40) & 0x0000ff0000000000;
    // sig |= ((uint64_t) 'R' << 48) & 0x00ff000000000000;
    // sig |= ((uint64_t) ' ' << 56) & 0xff00000000000000;
    memcpy(&sig, "RSD PTR ", 8);

    // 1. search for RSDP in first KB of EBDA
    uint64_t *addr = (uint64_t *) ebda_addr;
    // for (uint64_t *addr = (uint64_t *) ebda_addr; addr < ebda_addr + 1024; addr += 2) {
    for (int i = 0; i < 128; i += 2) {
        if (sig == addr[i]) {
            return &addr[i];
        }
    }

    // 2. search for RSDP in highest base mem
    for (uint64_t *addr = (uint64_t *) 0xe0000; addr < 0x100000; addr += 2) {
        if (sig == *addr) {
            return addr;
        }
    }

    return 0;
}

char gbuf[33];

int is_rsdp_1_valid(rsdp_1_t *rsdp) {
    uint8_t *p = rsdp;
    uint8_t sum = 0;
    for (int i = 0; i < sizeof(rsdp_1_t); ++i) {
        sum += p[i];
    }
    print(u32_to_str(sum, gbuf, 10));
    p[8] = 0;
    println(u32_to_str(p[0], gbuf, 10));
    return sum == 0;
}

int is_rsdp_2_valid(rsdp_2_t *rsdp) {
    if (is_rsdp_1_valid(&(rsdp->rsdp_1))) {
        uint8_t *p = &(rsdp->length);
        uint8_t sum = 0;
        for (int i = 0; i < rsdp->length - sizeof(rsdp_1_t); ++i) {
            sum += p[i];
        }
        println(u32_to_str(sum, gbuf, 10));
        return sum == 0;
    }
    return 0;
}

void acpi_init() {
    char buf[33];
    uint64_t addr = find_rsdp();
    if (!addr) {
        println("This computer has no ACPI!");
        return;
    }
    print("ACPI RSDP addr is ");
    println(u64_to_str(addr, buf, 10));
    uint8_t sum;
    if (((rsdp_1_t *) addr)->revision != 0) {
        rsdp_2_t *rsdp = (rsdp_2_t *) addr;
        if (!is_rsdp_2_valid(rsdp)) {
            println("RSDP 2.0 is not valid.");
        }
    } else {
        rsdp_1_t *rsdp = (rsdp_1_t *) addr;
        println(rsdp->signature);
        if (!is_rsdp_1_valid(rsdp)) {
            println("RSDP 1.0 is not valid.");
        }
    }
}