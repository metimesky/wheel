#include "interrupt.h"
#include "entries.h"
#include <utilities/env.h>
#include <utilities/cpu.h>
#include <utilities/clib.h>
#include <utilities/logging.h>
#include <drivers/acpi/acpi.h>

////////////////////////////////////////////////////////////////////////////////
/// IDT
////////////////////////////////////////////////////////////////////////////////

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
interrupt_handler interrupt_handler_table[INT_NUM];

// fill IDT entry with given handler address
static void fill_idt_entry(idt_entry_t *entry, void *handler) {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

// fill IDT entries and load IDTR
static void idt_init() {
    // clear IDT with zero
    memset(idt, 0, sizeof(idt));

    // internal exceptions, vector 0~31
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
    fill_idt_entry(&idt[20], isr20);
    fill_idt_entry(&idt[21], isr21);
    fill_idt_entry(&idt[22], isr22);
    fill_idt_entry(&idt[23], isr23);
    fill_idt_entry(&idt[24], isr24);
    fill_idt_entry(&idt[25], isr25);
    fill_idt_entry(&idt[26], isr26);
    fill_idt_entry(&idt[27], isr27);
    fill_idt_entry(&idt[28], isr28);
    fill_idt_entry(&idt[29], isr29);
    fill_idt_entry(&idt[30], isr30);
    fill_idt_entry(&idt[31], isr31);

    // external interrupts from old PIC - master
    fill_idt_entry(&idt[32], isr32);
    fill_idt_entry(&idt[33], isr33);
    fill_idt_entry(&idt[34], isr34);
    fill_idt_entry(&idt[35], isr35);
    fill_idt_entry(&idt[36], isr36);
    fill_idt_entry(&idt[37], isr37);
    fill_idt_entry(&idt[38], isr38);
    fill_idt_entry(&idt[39], isr39);

    // external interrupts from old PIC - slave
    fill_idt_entry(&idt[40], isr40);
    fill_idt_entry(&idt[41], isr41);
    fill_idt_entry(&idt[42], isr42);
    fill_idt_entry(&idt[43], isr43);
    fill_idt_entry(&idt[44], isr44);
    fill_idt_entry(&idt[45], isr45);
    fill_idt_entry(&idt[46], isr46);
    fill_idt_entry(&idt[47], isr47);

    idtr.base = (uint64_t) idt;
    idtr.limit = INT_NUM * sizeof(idt_entry_t) - 1;
    __asm__ __volatile__("lidt (%0)" :: "a"(&idtr));
    load_idtr(&idtr);   // defined in library/cpu.asm
}

////////////////////////////////////////////////////////////////////////////////
/// TSS
////////////////////////////////////////////////////////////////////////////////

// structure of long mode TSS
struct tss {
    uint32_t reserved_1;
    uint32_t rsp0_l;
    uint32_t rsp0_h;
    uint32_t rsp1_l;
    uint32_t rsp1_h;
    uint32_t rsp2_l;
    uint32_t rsp2_h;
    uint32_t reserved_2;
    uint32_t reserved_3;
    uint32_t ist1_l;
    uint32_t ist1_h;
    uint32_t ist2_l;
    uint32_t ist2_h;
    uint32_t ist3_l;
    uint32_t ist3_h;
    uint32_t ist4_l;
    uint32_t ist4_h;
    uint32_t ist5_l;
    uint32_t ist5_h;
    uint32_t ist6_l;
    uint32_t ist6_h;
    uint32_t ist7_l;
    uint32_t ist7_h;
    uint32_t reserved_4;
    uint32_t reserved_5;
    uint16_t reserved_6;
    uint16_t io_map_base;
} __attribute__((packed));
typedef struct tss tss_t;

// the top address of default kernel stack, defined in boot.asm
extern char kernel_stack_top;
extern uint64_t gdt[];
tss_t tss;      // scheduler may need to access tss

static void tss_init() {
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

////////////////////////////////////////////////////////////////////////////////
/// Interrupt
////////////////////////////////////////////////////////////////////////////////

extern void unwind_from(uint64_t rbp);

// Default interrupt handler, just print related information
static void default_interrupt_handler(int vec, interrupt_context_t *ctx) {
    static const char* sym[] = {
        "DE", "DB", "NMI", "BP", "OF", "BR", "UD", "NM",
        "DF", "><", "TS", "NP", "SS", "GP", "PF", "><",
        "MF", "AC", "MC", "XF", "><", "><", "><", "><",
        "><", "><", "><", "><", "><", "><", "SX", "><"
    };

    // print interrupt info
    if (vec < 32) {
        log("Exception #%s, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x, err=%x",
            sym[vec], ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags, ctx->err_code);
        if (vec == 14) {
            uint64_t virt;
            __asm__ __volatile__("mov %%cr2, %0" : "=r"(virt));
            log("Page Fault virtual address %x", virt);
        }
    } else {
        log("Interrupt #%d, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x",
            vec, ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags);
    }
    
    // stack unwinding
    unwind_from(ctx->rbp);
    
    // stop here
    while (true) {}
}

void interrupt_init() {
    // initially, fill all interrupt handler as the default one
    for (int i = 0; i < INT_NUM; ++i) {
        interrupt_handler_table[i] = default_interrupt_handler;
    }

    // setup tss first
    tss_init();

    // create and activate IDT
    idt_init();

    // disable 8258A by masking all interrupts
    //disable_8259();

    // enabling local APIC by setting bit 8 of spurious interrupt vector reg
    // TODO: the location of local APIC should be queried from MADT
    //acpi_init();
    // if (madt_present) {
    //     io_apic_init(io_apic_base[0]);
    //     local_apic_init(local_apic_base);
    // }

    // enable system-wide interrupt
    //__asm__("sti");
}

// these two functions even can be called before interrupt was initialized

// replace current interrupt handler with the one given
void interrupt_install_handler(int vec, interrupt_handler func) {
    if (0 <= vec && vec < INT_NUM) {
        interrupt_handler_table[vec] = func;
    }
}

// rewrite the interrupt handler with the default one
void interrupt_remove_handler(int vec) {
    if (0 <= vec && vec < INT_NUM) {
        interrupt_handler_table[vec] = default_interrupt_handler;
    }
}
