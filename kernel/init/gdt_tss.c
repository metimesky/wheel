#include "gdt_tss.h"
#include <memory/memory.h>
#include <drivers/apic/apic.h>

// the top address of default kernel stack, defined in boot.asm
extern char kernel_stack_top;
extern uint64_t gdt[];

static tss_t tss;      // scheduler may need to access tss

// defined in gdt_tss.asm
extern void reload_gdtr(uint64_t gdtr);

// void gdt_tss_init() {
//     // gdt_ptr;
//     // alloc_static();
// }

// initialize the TSS of BSP
void bsp_tss_init() {
    uint64_t rsp0 = (uint64_t) &kernel_stack_top;
    tss.rsp0_l = rsp0 & 0xffffffff;
    tss.rsp0_h = (rsp0 >> 32) & 0xffffffff;

    uint64_t base_addr = (uint64_t) &tss;
    uint64_t seg_limit = sizeof(tss);

    // lower half of tss descriptor
    gdt[5] = 0UL;
    gdt[5] |= seg_limit & 0xffffUL;                     // seg limit [15:0]
    gdt[5] |= (base_addr << 16) & 0x000000ffffff0000UL; // base addr [23:0]
    gdt[5] |= 0x0000890000000000UL;                     // present, 64bit tss
    gdt[5] |= ((seg_limit << 32) & 0x000f000000000000UL);   // limit [19:16]
    gdt[5] |= ((base_addr << 32) & 0xff00000000000000UL);   // base [31:24]

    // higher half of tss descriptor
    gdt[6] = 0UL;
    gdt[6] |= (base_addr >> 32) & 0xffffffff;

    __asm__ __volatile__("ltr %%ax" :: "a"(0x28));
}

void tss_set_rsp0(uint64_t rsp) {
    tss.rsp0_l = rsp & 0xffffffff;
    tss.rsp0_h = (rsp >> 32) & 0xffffffff;
}