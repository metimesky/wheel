#include "interrupt.h"
#include <env.h>
#include <utils.h>

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

void handler() {
    raw_write("INTERRUPT!", 0x1e, 23*80);
    while (1) {}
}

idt_entry_t idt[INT_NUM];
idt_ptr_t idtr;

void idt_init() {
    idtr.base = (uint64_t) idt;
    idtr.limit = INT_NUM * sizeof(idt_entry_t) - 1;
    for (int i = 0; i < INT_NUM; ++i) {
        idt[i].selector = 8;
        idt[i].offset_low = ((uint64_t) &handler) & 0xffff;
        idt[i].offset_mid = ((uint64_t) &handler >> 16) & 0xffff;
        idt[i].offset_high = ((uint64_t) &handler >> 32) & 0xffffffff;
        idt[i].attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    }
    load_idtr(&idtr);
}

#define PIC1        0x20        /* IO base address for master PIC */
#define PIC2        0xA0        /* IO base address for slave PIC */
#define PIC1_CMD    PIC1
#define PIC1_DAT    (PIC1+1)
#define PIC2_CMD    PIC2
#define PIC2_DAT    (PIC2+1)

#define ICW1_ICW4   0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08        /* Level triggered (edge) mode */
#define ICW1_INIT   0x10        /* Initialization - required! */

#define ICW4_8086   0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO   0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C        /* Buffered mode/master */
#define ICW4_SFNM   0x10        /* Special fully nested (not) */

void remap_8259(int master_offset, int slave_offset) {
    unsigned char a1, a2;

    a1 = in_byte(PIC1_DAT);
    a2 = in_byte(PIC2_DAT);

    out_byte(PIC1_CMD, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    out_byte(PIC2_CMD, ICW1_INIT+ICW1_ICW4);
    io_wait();
    out_byte(PIC1_DAT, master_offset);      // ICW2: Master PIC vector offset
    io_wait();
    out_byte(PIC2_DAT, slave_offset);       // ICW2: Slave PIC vector offset
    io_wait();
    out_byte(PIC1_DAT, 4);                  // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    out_byte(PIC2_DAT, 2);                  // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out_byte(PIC1_DAT, ICW4_8086);
    io_wait();
    out_byte(PIC2_DAT, ICW4_8086);
    io_wait();

    out_byte(PIC1_DAT, a1);   // restore saved masks.
    out_byte(PIC2_DAT, a2);
}

void init_8259() {
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

    out_byte(PIC1_DAT, 0xff);
    out_byte(PIC2_DAT, 0xff);
}

void interrupt_init() {
    idt_init();
    init_8259();
}