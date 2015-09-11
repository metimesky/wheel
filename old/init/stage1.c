#include <stdint.h>
#include "entry.h"

// low level printing function
static void write(const char *str, char attr) {
    static char* const video = (char*) 0xb8000;
    static int cursor = 0;
    for (int i = 0; str[i]; ++i) {
        if (str[i] == '\n') {
            cursor += 79;
            cursor -= cursor % 80;
        } else {
            video[2 * cursor] = str[i];
            video[2 * cursor + 1] = attr;
            ++cursor;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// GDT

#define SEG_TYPE(x)  ((x) << 0x04)          // descriptor type
#define SEG_PRES(x)  ((x) << 0x07)          // present
#define SEG_SAVL(x)  ((x) << 0x0c)          // available for system use
#define SEG_LONG(x)  ((x) << 0x0d)          // long mode
#define SEG_SIZE(x)  ((x) << 0x0e)          // size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)  ((x) << 0x0f)          // granularity
#define SEG_PRIV(x) (((x) &  0x03) << 0x05) // privilege level (0 - 3)

#define CODE_PL0 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(0) | 0x0a  // execute/read code segment, non-confirming

#define DATA_PL0 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(0) | 0x02  // read/write data segment

#define CODE_PL3 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(3) | 0x0a  // execute/read code segment, non-confirming

#define DATA_PL3 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(3) | 0x02  // read/write data segment

#define GDT_NUM 7
static uint64_t gdt[GDT_NUM];

// defined in lib/load_gdtr.asm
extern void load_gdtr(uint64_t);

void init_gdt() {
    gdt[0] = 0UL;
    gdt[1] = 0UL | CODE_PL0;    // kernel code segment
    gdt[2] = 0UL | DATA_PL0;    // kernel data segment
    gdt[3] = 0UL | CODE_PL3;    // user code segment
    gdt[4] = 0UL | DATA_PL3;    // user data segment

    struct {
        uint16_t size;
        uint64_t base;
    } __attribute__((packed)) gdtr;
    gdtr.base = (uint64_t) gdt;
    gdtr.size = 5 * sizeof(uint64_t) - 1;
    __asm__ ("lgdt (%%rax)" :: "a"(&gdtr));
    // load_gdtr((uint64_t) &gdtr);
}

////////////////////////////////////////////////////////////////////////////////
// IDT

// 64-bit IDT entry structure
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

#define IDT_NUM 64
idt_entry_t idt[IDT_NUM];
void* interrupt_handler_table[IDT_NUM];

void fill_idt_entry(idt_entry_t *entry, void *handler) {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

void common_handler() {
    while (1) {}
}

void init_idt() {
    for (int i = 0; i < IDT_NUM; ++i) {
        interrupt_handler_table[i] = (void*) common_handler;
    }

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

    struct {
        uint16_t size;
        uint64_t base;
    } __attribute__((packed)) idtr;
    idtr.base = (uint64_t) idt;
    idtr.size = 5 * sizeof(idt_entry_t) - 1;
    __asm__ ("lidt (%%rax)" :: "a"(&idtr));
}

////////////////////////////////////////////////////////////////////////////////
// Main function of stage1

void stage1(uint32_t eax, uint32_t ebx) {
    write("[0] Initializing new GDT ... ", 0x0f);
    init_gdt();
    write("done\n", 0x0a);

    write("[1] Initializing IDT ... ", 0x0f);
    init_idt();
    write("done\n", 0x0a);
}
