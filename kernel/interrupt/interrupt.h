#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__ 1

/* This is only a framework for interrupt, the individual interrupt handler
 * should be implemented in other modules.
 */

#include <utilities/env.h>

#define INTERRUPT_NUM 128

// must be compliant with entries.asm
struct interrupt_context {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t err_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));
typedef struct interrupt_context interrupt_context_t;

typedef void (*interrupt_handler)(int vec, interrupt_context_t *ctx);

extern void interrupt_init();
extern void interrupt_install_handler(int vec, interrupt_handler func);
extern void interrupt_remove_handler(int vec);

#endif // __INTERRUPT_H__