#include <types.h>
#include "protect.h"

// data definition
gdt_ptr_t gdt_ptr;
idt_ptr_t idt_ptr;
uint64_t gdt[GDT_SIZE];
uint64_t idt[IDT_SIZE];
tss_t tss;

// only low 20 bit of limit is valid
static uint64_t create_gdt_descriptor(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor = 0;

    // create the high 32 bit segment
    descriptor  =  limit       & 0x000f0000;    // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00f0ff00;    // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000ff;    // set base bits 23:16
    descriptor |=  base        & 0xff000000;    // set base bits 31:24

    // shift by 32 to allow for low part of segment
    descriptor <<= 32;

    // create the low 32 bit segment
    descriptor |= base  << 16;                  // set base bits 15:0
    descriptor |= limit  & 0x0000ffff;          // set limit bits 15:0

    return descriptor;
}

static uint64_t create_idt_descriptor(uint16_t selector, uint32_t offset, uint8_t flag) {
    uint64_t descriptor = 0;

    // create the high 32 bit segment
    descriptor |=  offset     & 0xffff0000;
    descriptor |= (flag << 8) & 0x0000ff00;

    // shift by 32 to allow for low part of segment
    descriptor <<= 32;

    // create the low 32 bit segment
    descriptor |= (selector << 16) & 0xffff0000;
    descriptor |=  offset          & 0x0000ffff;

    return descriptor;
}

void setup_gdt() {
    gdt[0] = create_gdt_descriptor(0, 0, 0);                    // null descriptor
    gdt[1] = create_gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);   // kernel code segment
    gdt[2] = create_gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);   // kernel data segment
    gdt[3] = create_gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);   // user code segment
    gdt[4] = create_gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);   // user data segment
    gdt[5] = create_gdt_descriptor((uint32_t)&tss, sizeof(tss), GDT_TSS); // TSS segment

    gdt_ptr.base  = (uint32_t)gdt;
    gdt_ptr.limit = GDT_SIZE*sizeof(uint64_t) - 1;
}

void setup_tss() {
    //
}

void handler() {
    char *video = (char*)0xb8000;
    for (int i = 0; i < 10; ++i) {
        video[160+2*i] = 'I';
        video[161+2*i] = 0x0e;
    }
    while (1) {}
}

void setup_idt() {
    uint64_t desc = create_idt_descriptor(8, (uint32_t)handler, IDT_INT_PL0);
    idt[0x40] = create_idt_descriptor(8, (uint32_t)handler, IDT_INT_PL0);

    idt_ptr.base  = (uint32_t)idt;
    idt_ptr.limit = IDT_SIZE*sizeof(uint64_t) - 1;
}
