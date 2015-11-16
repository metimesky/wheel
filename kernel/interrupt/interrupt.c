#include "interrupt.h"
#include "entries.h"
#include <common/stdhdr.h>
#include <common/util.h>

#define PIC1            0x20        /* IO base address for master PIC */
#define PIC2            0xA0        /* IO base address for slave PIC */
#define PIC1_CMD        PIC1
#define PIC1_DAT        (PIC1+1)
#define PIC2_CMD        PIC2
#define PIC2_DAT        (PIC2+1)

#define ICW1_ICW4       0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08        /* Level triggered (edge) mode */
#define ICW1_INIT       0x10        /* Initialization - required! */

#define ICW4_8086       0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C        /* Buffered mode/master */
#define ICW4_SFNM       0x10        /* Special fully nested (not) */

void disable_8259() {
    out_byte(PIC1_CMD, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    out_byte(PIC2_CMD, ICW1_INIT+ICW1_ICW4);
    io_wait();
    out_byte(PIC1_DAT, 32);     // ICW2: Master PIC vector offset
    io_wait();
    out_byte(PIC2_DAT, 40);     // ICW2: Slave PIC vector offset
    io_wait();
    out_byte(PIC1_DAT, 4);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    out_byte(PIC2_DAT, 2);      // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out_byte(PIC1_DAT, ICW4_8086);
    io_wait();
    out_byte(PIC2_DAT, ICW4_8086);
    io_wait();

    out_byte(PIC1_DAT, 0xff);   // mask all interrupts on master chip
    out_byte(PIC2_DAT, 0xff);   // mask all interrupts on slave chip
}

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

idt_entry_t idt[INT_NUM];
idt_ptr_t idtr;
void* interrupt_handler_table[INT_NUM];

void default_interrupt_handler() {
    log("interrupt");
    while (true) {}
}

void fill_idt_entry(idt_entry_t *entry, void *handler) {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

void idt_init() {
    // initially, fill all interrupt handler as the default one
    for (int i = 0; i < INT_NUM; ++i) {
        interrupt_handler_table[i] = (void*) default_interrupt_handler;
    }

    // clear IDT with zero
    memset(idt, 0, sizeof(idt));

    // init entry 0~19
    fill_idt_entry(&idt[0], isr0);
    fill_idt_entry(&idt[1], isr1);
    fill_idt_entry(&idt[2], isr2);
    fill_idt_entry(&idt[3], isr3);
    fill_idt_entry(&idt[4], isr4);
    fill_idt_entry(&idt[5], isr5);
    fill_idt_entry(&idt[6], isr6);
    fill_idt_entry(&idt[7], isr7);
    fill_idt_entry(&idt[8], isr8);
    fill_idt_entry(&idt[9], isr9);
    fill_idt_entry(&idt[10], isr10);
    fill_idt_entry(&idt[11], isr11);
    fill_idt_entry(&idt[12], isr12);
    fill_idt_entry(&idt[13], isr13);
    fill_idt_entry(&idt[14], isr14);
    fill_idt_entry(&idt[15], isr15);
    fill_idt_entry(&idt[16], isr16);
    fill_idt_entry(&idt[17], isr17);
    fill_idt_entry(&idt[18], isr18);
    fill_idt_entry(&idt[19], isr19);

    // init entry 30
    fill_idt_entry(&idt[30], isr30);

    // external interrupts
    fill_idt_entry(&idt[32], isr32);  // clock
    fill_idt_entry(&idt[33], isr33);  // keyboard

    idtr.base = (uint64_t) idt;
    idtr.limit = INT_NUM * sizeof(idt_entry_t) - 1;
    load_idtr(&idtr);   // defined in library/cpu.asm
}

void interrupt_init() {
    // create and activate IDT
    idt_init();

    // disable 8258A by masking all interrupts
    disable_8259();

    // enabling local APIC by setting bit 8 of spurious interrupt vector reg
    // TODO: the location of local APIC should be queried from MADT
    uint32_t val = *((uint32_t *) 0xfee000f0);
    val |= 1 << 8;
    *((uint32_t *) 0xfee000f0) = val;
}