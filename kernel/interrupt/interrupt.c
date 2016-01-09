#include "interrupt.h"
#include "idt.h"
#include "tss.h"
#include <utilities/env.h>
#include <utilities/cpu.h>
// #include <utilities/clib.h>
#include <utilities/logging.h>
#include <drivers/acpi/acpi.h>

interrupt_handler interrupt_handler_table[INTERRUPT_NUM];

extern void unwind_from(uint64_t rbp);

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

// this function only initialize interrupt framework, actual interrupt is
// being still disabled.
void interrupt_init() {
    // initially, fill all interrupt handler as the default one
    for (int i = 0; i < INTERRUPT_NUM; ++i) {
        interrupt_handler_table[i] = default_interrupt_handler;
    }

    // setup tss first
    tss_init();

    // create and activate IDT
    idt_init();
}

// these two functions even can be called before interrupt was initialized

// replace current interrupt handler with the one given
void interrupt_install_handler(int vec, interrupt_handler func) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        interrupt_handler_table[vec] = func;
    }
}

// rewrite the interrupt handler with the default one
void interrupt_remove_handler(int vec) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        interrupt_handler_table[vec] = default_interrupt_handler;
    }
}
