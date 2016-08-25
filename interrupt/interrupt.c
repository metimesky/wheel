#include <interrupt.h>
#include <wheel.h>
#include "idt.h"
#include "pic.h"
#include "apic.h"

#include <drivers/console.h>

interrupt_handler_t interrupt_handler_table[INTERRUPT_NUM];

// Default interrupt handler, just print related information
static void default_interrupt_handler(int vec, interrupt_context_t *ctx) {
    // exception mnemonics
    static const char* sym[] = {
        "DE", "DB", "NMI", "BP", "OF", "BR", "UD", "NM",
        "DF", "><", "TS", "NP", "SS", "GP", "PF", "><",
        "MF", "AC", "MC", "XF", "><", "><", "><", "><",
        "><", "><", "><", "><", "><", "><", "SX", "><"
    };

    // print interrupt info
    if (vec < 32) {
        console_print("Exception #%s, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x, err=%x",
            sym[vec], ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags, ctx->err_code);
        if (vec == 14) {
            uint64_t virt;
            __asm__ __volatile__("mov %%cr2, %0" : "=r"(virt));
            console_print("Page Fault virtual address %x", virt);
        }
    } else {
        console_print("Interrupt #%d, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x",
            vec, ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags);
    }
    
    // stack unwinding
    // unwind_from(ctx->rbp);
    
    // stop here
    while (true) {}
}

// this function only initialize interrupt framework, actual interrupt is
// being still disabled.
void interrupt_init() {
    // initially, fill all interrupt handler as the default one
    for (int i = 0; i < INTERRUPT_NUM; ++i) {
        interrupt_handler_table[i] = default_interrupt_handler;
    }

    // setup tss first
    // tss_init();

    // create and activate IDT
    idt_init();

    // 初始化PIC，默认所有IRQ均为禁用状态
    pic_init();

    // 初始化APIC
    apic_preinit();
    io_apic_init();
    local_apic_init();

    // enable interrupt
    // __asm__ __volatile__("sti");
}

void interrupt_init_ap() {
    ;
}

// 获取中断处理函数指针
interrupt_handler_t interrupt_get_handler(int vec) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        return interrupt_handler_table[vec];
    }
}

// 设置中断处理函数指针
void interrupt_set_handler(int vec, interrupt_handler_t cb) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        if (cb == NULL) {
            interrupt_handler_table[vec] = default_interrupt_handler;
        } else {
            interrupt_handler_table[vec] = cb;
        }
    }
}
