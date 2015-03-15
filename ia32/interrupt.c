#include <types.h>
#include "protect.h"
#include "utils.h"

#define MASTER_CMD  0x20
#define MASTER_DATA 0x21
#define SLAVE_CMD   0xa0
#define SLAVE_DATA  0xa1

idt_ptr_t idt_ptr;
uint64_t idt[IDT_SIZE];

void setup_8259A() {
    out_byte(MASTER_CMD, 0x11);
    out_byte(SLAVE_CMD,  0x11);

    out_byte(MASTER_DATA, 0x20);    // mapping irq0 to 32
    out_byte(SLAVE_DATA,  0x28);    // mapping irq8 to 40

    out_byte(MASTER_DATA, 4);       // tell master PIC irq2 connects to the slave PIC
    out_byte(SLAVE_DATA,  2);       // tell slave PIC its cascade identity

    out_byte(MASTER_DATA, 1);
    out_byte(SLAVE_DATA,  1);

    out_byte(MASTER_DATA, 0xff);    // mask all interrupts
    out_byte(SLAVE_DATA,  0xff);    // mask all interrupts
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

// called by assembly stub
void exception_handler(int vec, uint32_t err, uint32_t eip, uint32_t cs, uint32_t eflags) {
    /*static*/ char *str = "exception occurred!";
    char *video = (char*)0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2*i] = str[i];
        video[2*i+1] = 0x1e;
    }
}

extern void exp0();
extern void exp1();
extern void exp2();
extern void exp3();
extern void exp4();
extern void exp5();
extern void exp6();
extern void exp7();
extern void exp8();
extern void exp9();
extern void exp10();
extern void exp11();
extern void exp12();
extern void exp13();
extern void exp14();
extern void exp15();
extern void exp16();
extern void exp17();
extern void exp18();
extern void exp19();
extern void exp20();

// extern void irq0();
// extern void irq1();
// extern void irq2();
// extern void irq3();
// extern void irq4();
// extern void irq5();
// extern void irq6();
// extern void irq7();
// extern void irq8();
// extern void irq9();
// extern void irq10();
// extern void irq11();
// extern void irq12();
// extern void irq13();
// extern void irq14();
// extern void irq15();

void setup_idt() {
    idt[ 0] = create_idt_descriptor(8, (uint32_t) exp0, IDT_INT_PL0);
    idt[ 1] = create_idt_descriptor(8, (uint32_t) exp1, IDT_INT_PL0);
    idt[ 2] = create_idt_descriptor(8, (uint32_t) exp2, IDT_INT_PL0);
    idt[ 3] = create_idt_descriptor(8, (uint32_t) exp3, IDT_INT_PL0);
    idt[ 4] = create_idt_descriptor(8, (uint32_t) exp4, IDT_INT_PL0);
    idt[ 5] = create_idt_descriptor(8, (uint32_t) exp5, IDT_INT_PL0);
    idt[ 6] = create_idt_descriptor(8, (uint32_t) exp6, IDT_INT_PL0);
    idt[ 7] = create_idt_descriptor(8, (uint32_t) exp7, IDT_INT_PL0);
    idt[ 8] = create_idt_descriptor(8, (uint32_t) exp8, IDT_INT_PL0);
    idt[ 9] = create_idt_descriptor(8, (uint32_t) exp9, IDT_INT_PL0);
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
    
    for (int i = 21; i < 32; ++i) { idt[i] = 0; }

    idt_ptr.base  = (uint32_t)idt;
    idt_ptr.limit = IDT_SIZE*sizeof(uint64_t) - 1;

    load_idtr(&idt_ptr);
}

void setup_interrupt() {
    setup_8259A();
    setup_idt();
}
