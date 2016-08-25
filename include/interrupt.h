#ifndef __INTERRUPT__
#define __INTERRUPT__

#include <wheel.h>

// 64位模式下最多128个中断
#define INTERRUPT_NUM 128

#define IRQ_VEC_BASE 32     // PIC中断映射到的IRQ编号
#define LVT_VEC_BASE 48     // Local APIC LVT映射到的IRQ编号
#define SVR_VEC_NUM  63     // spurious-interrupt vector register的向量号
#define GSI_VEC_BASE 64     // IO APIC的Global Int映射的中断向量号

// 终端时保存的上下文，必须和entries.asm中的代码一致
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

typedef void (*interrupt_handler_t)(int vec, interrupt_context_t *ctx);

extern void interrupt_init();

extern interrupt_handler_t interrupt_get_handler(int vec);
extern void interrupt_set_handler(int vec, interrupt_handler_t cb);

extern void pic_send_eoi(int irq);
extern void local_apic_send_eoi();

#endif