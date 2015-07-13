#include "interrupt.h"
#include <env.h>
#include <utils.h>
#include <string.h>

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

void int_handler(int vec_no, uint32_t errcode, uint64_t rip, uint64_t rflags, uint64_t rsp, uint64_t ss) {
    char buf[128];
    sprintf(buf, "INTERRUPT #%d! err: %d, rip: %x, rflags: %x, rsp: %x, ret ss: %x", vec_no, errcode, rip, rflags, rsp, ss);
    raw_write(buf, 0x1e, 23*80);
    while (1) {}
}

// void clock_handler(int vec_no, uint32_t errcode, uint64_t rip, uint64_t rflags, uint64_t rsp, uint64_t ss) {
//     char buf[128];
//     sprintf(buf, "CLOCK #%d! err: %d, rip: %x, rflags: %x, rsp: %x, ret ss: %x", vec_no, errcode, rip, rflags, rsp, ss);
//     raw_write(buf, 0x1e, 22*80);
//     char *video = (char*)0xb8000;
//     ++video[0];
// }

idt_entry_t idt[INT_NUM];
idt_ptr_t idtr;
void* interrupt_handler_table[INT_NUM];

extern void hwint0();
extern void hwint1();
extern void hwint2();
extern void hwint3();
extern void hwint4();
extern void hwint5();
extern void hwint6();
extern void hwint7();
extern void hwint8();
extern void hwint9();
extern void hwint10();
extern void hwint11();
extern void hwint12();
extern void hwint13();
extern void hwint14();
extern void hwint15();
extern void hwint16();
extern void hwint17();
extern void hwint18();
extern void hwint19();
extern void hwint30();

extern void hwint32();
extern void hwint33();
extern void hwint34();
extern void hwint35();
extern void hwint64();
extern void clock_entry();

void fill_idt_entry(idt_entry_t *entry, void *handler) {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

void clock_handler(
    uint64_t no,
    uint64_t rip,
    uint16_t cs,
    uint64_t rflags,
    uint64_t rsp,
    uint16_t ss
) {
    char buf[128];
    static char *video = (char*) 0xb8000;
    ++video[0];
    sprintf(buf, "CLOCK: %x!, cs:rip => %x:%x, ss:rsp => %x:%x, rflags: %x", no, cs, rip, ss, rsp, rflags);
    raw_write(buf, 0x0c, 24*80);
}

void idt_init() {
    for (int i = 0; i < INT_NUM; ++i) {
        interrupt_handler_table[i] = (void*) int_handler;
    }

    memset(idt, 0, sizeof(idt));

    // init entry 0~19
    fill_idt_entry(&idt[0], hwint0);
    fill_idt_entry(&idt[1], hwint1);
    fill_idt_entry(&idt[2], hwint2);
    fill_idt_entry(&idt[3], hwint3);
    fill_idt_entry(&idt[4], hwint4);
    fill_idt_entry(&idt[5], hwint5);
    fill_idt_entry(&idt[6], hwint6);
    fill_idt_entry(&idt[7], hwint7);
    fill_idt_entry(&idt[8], hwint8);
    fill_idt_entry(&idt[9], hwint9);
    fill_idt_entry(&idt[10], hwint10);
    fill_idt_entry(&idt[11], hwint11);
    fill_idt_entry(&idt[12], hwint12);
    fill_idt_entry(&idt[13], hwint13);
    fill_idt_entry(&idt[14], hwint14);
    fill_idt_entry(&idt[15], hwint15);
    fill_idt_entry(&idt[16], hwint16);
    fill_idt_entry(&idt[17], hwint17);
    fill_idt_entry(&idt[18], hwint18);
    fill_idt_entry(&idt[19], hwint19);

    // init entry 30
    fill_idt_entry(&idt[30], hwint30);

    // external interrupts
    fill_idt_entry(&idt[32], clock_entry);  // clock
    interrupt_handler_table[32] = (void*) clock_handler;
    //fill_idt_entry(&idt[33], hwint33);  // keyboard

    fill_idt_entry(&idt[64], clock_entry);
    interrupt_handler_table[64] = (void*) clock_handler;

    idtr.base = (uint64_t) idt;
    idtr.limit = INT_NUM * sizeof(idt_entry_t) - 1;
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

    out_byte(PIC1_DAT, 0xfe);
    out_byte(PIC2_DAT, 0xff);
}

void interrupt_init() {
    idt_init();
    init_8259();
}