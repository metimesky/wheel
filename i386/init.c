#include <types.h>
#include <libk.h>
#include "i386.h"
#include "interrupt.h"
#include "liba.h"

#define GDT_SIZE 6
#define IDT_SIZE 256

gdt_ptr_t gdt_ptr;
idt_ptr_t idt_ptr;
uint64_t gdt[GDT_SIZE];
uint64_t idt[IDT_SIZE];
tss_t tss;

static uint64_t make_gdt_descriptor(uint32_t base, uint32_t limit, uint16_t flag) {
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

void init(uint32_t eax, uint32_t ebx) {
    if (0x2badb002!=eax) {
        // error, bootloader not compliant
    }
    
    // initialize gdt
    memset(gdt, 0, sizeof(gdt));
    gdt[0] = make_gdt_descriptor(0, 0, 0);                      // null descriptor
    gdt[1] = make_gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);     // kernel code segment
    gdt[2] = make_gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);     // kernel data segment
    gdt[3] = make_gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);     // user code segment
    gdt[4] = make_gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);     // user data segment
    gdt[5] = make_gdt_descriptor((uint32_t)&tss, sizeof(tss), GDT_TSS); // TSS segment

    gdt_ptr.base  = (uint32_t)gdt;
    gdt_ptr.limit = GDT_SIZE*sizeof(uint64_t) - 1;
    
    load_gdtr(&gdt_ptr);

    // initialize 8259A
#define MASTER_CMD  0x20
#define MASTER_DATA 0x21
#define SLAVE_CMD   0xa0
#define SLAVE_DATA  0xa1
    out_byte(MASTER_CMD,  0x11);    io_wait();  // ICW1, master 8259A
    out_byte(SLAVE_CMD,   0x11);    io_wait();  // ICW1, slave 8259A
    out_byte(MASTER_DATA, 0x20);    io_wait();  // ICW2, master 8259A, mapping irq0 to 32
    out_byte(SLAVE_DATA,  0x28);    io_wait();  // ICW2, slave 8259A, mapping irq8 to 40
    out_byte(MASTER_DATA, 4);       io_wait();  // ICW3, master 8259A, PIN2 connects to the slave PIC
    out_byte(SLAVE_DATA,  2);       io_wait();  // ICW3, slave 8259A, connects to PIN2 of master chip
    out_byte(MASTER_DATA, 1);       io_wait();  // ICW4, master 8259A
    out_byte(SLAVE_DATA,  1);       io_wait();  // ICW4, salve 8259A
    out_byte(MASTER_DATA, 0xfe);    io_wait();  // OCW1, master 8259A, mask all interrupts
    out_byte(SLAVE_DATA,  0xff);    io_wait();  // OCW1, slave 8259A, mask all interrupts

    // initialize idt
    memset(idt, 0, sizeof(idt));
    idt[ 0] = create_idt_descriptor(8, (uint32_t)exp0,  IDT_INT_PL0);
    idt[ 1] = create_idt_descriptor(8, (uint32_t)exp1,  IDT_INT_PL0);
    idt[ 2] = create_idt_descriptor(8, (uint32_t)exp2,  IDT_INT_PL0);
    idt[ 3] = create_idt_descriptor(8, (uint32_t)exp3,  IDT_INT_PL0);
    idt[ 4] = create_idt_descriptor(8, (uint32_t)exp4,  IDT_INT_PL0);
    idt[ 5] = create_idt_descriptor(8, (uint32_t)exp5,  IDT_INT_PL0);
    idt[ 6] = create_idt_descriptor(8, (uint32_t)exp6,  IDT_INT_PL0);
    idt[ 7] = create_idt_descriptor(8, (uint32_t)exp7,  IDT_INT_PL0);
    idt[ 8] = create_idt_descriptor(8, (uint32_t)exp8,  IDT_INT_PL0);
    idt[ 9] = create_idt_descriptor(8, (uint32_t)exp9,  IDT_INT_PL0);
    idt[10] = create_idt_descriptor(8, (uint32_t)exp10, IDT_INT_PL0);
    idt[11] = create_idt_descriptor(8, (uint32_t)exp11, IDT_INT_PL0);
    idt[12] = create_idt_descriptor(8, (uint32_t)exp12, IDT_INT_PL0);
    idt[13] = create_idt_descriptor(8, (uint32_t)exp13, IDT_INT_PL0);
    idt[14] = create_idt_descriptor(8, (uint32_t)exp14, IDT_INT_PL0);
    idt[15] = create_idt_descriptor(8, (uint32_t)exp15, IDT_INT_PL0);
    idt[16] = create_idt_descriptor(8, (uint32_t)exp16, IDT_INT_PL0);
    idt[17] = create_idt_descriptor(8, (uint32_t)exp17, IDT_INT_PL0);
    idt[18] = create_idt_descriptor(8, (uint32_t)exp18, IDT_INT_PL0);
    idt[19] = create_idt_descriptor(8, (uint32_t)exp19, IDT_INT_PL0);
    idt[20] = create_idt_descriptor(8, (uint32_t)exp20, IDT_INT_PL0);

    idt[32] = create_idt_descriptor(8, (uint32_t)irq0,  IDT_INT_PL0);
    idt[33] = create_idt_descriptor(8, (uint32_t)irq1,  IDT_INT_PL0);
    idt[34] = create_idt_descriptor(8, (uint32_t)irq2,  IDT_INT_PL0);
    idt[35] = create_idt_descriptor(8, (uint32_t)irq3,  IDT_INT_PL0);
    idt[36] = create_idt_descriptor(8, (uint32_t)irq4,  IDT_INT_PL0);
    idt[37] = create_idt_descriptor(8, (uint32_t)irq5,  IDT_INT_PL0);
    idt[38] = create_idt_descriptor(8, (uint32_t)irq6,  IDT_INT_PL0);
    idt[39] = create_idt_descriptor(8, (uint32_t)irq7,  IDT_INT_PL0);
    idt[40] = create_idt_descriptor(8, (uint32_t)irq8,  IDT_INT_PL0);
    idt[41] = create_idt_descriptor(8, (uint32_t)irq9,  IDT_INT_PL0);
    idt[42] = create_idt_descriptor(8, (uint32_t)irq10, IDT_INT_PL0);
    idt[43] = create_idt_descriptor(8, (uint32_t)irq11, IDT_INT_PL0);
    idt[44] = create_idt_descriptor(8, (uint32_t)irq12, IDT_INT_PL0);
    idt[45] = create_idt_descriptor(8, (uint32_t)irq13, IDT_INT_PL0);
    idt[46] = create_idt_descriptor(8, (uint32_t)irq14, IDT_INT_PL0);
    idt[47] = create_idt_descriptor(8, (uint32_t)irq15, IDT_INT_PL0);

    idt_ptr.base  = (uint32_t)idt;
    idt_ptr.limit = IDT_SIZE*sizeof(uint64_t) - 1;

    load_idtr(&idt_ptr);

    // initialize tss
    tss.ss0 = 0x10;             // kernel data segment
    tss.iobase = sizeof(tss);   // no IO map

    load_tr(0x28);              // the selector of tss segment
}

void exception_dispatcher() {
    static char *video = (char*)0xb8000;
    video[158] = '!';
}

extern void schedule();

void interrupt_dispatcher() {
    static char *video = (char*)0xb8000;
    ++video[0];
    //schedule();
}