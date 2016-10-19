#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <wheel.h>
#include <percpu.h>
#include <drivers/acpi/acpi.h>

// 64位模式下最多256个中断
#define INTERRUPT_NUM 256

#define IRQ_VEC_BASE 32     // PIC中断映射到的IRQ编号
#define LVT_VEC_BASE 48     // Local APIC LVT映射到的IRQ编号
#define SVR_VEC_NUM  63     // spurious-interrupt vector register的向量号
#define GSI_VEC_BASE 64     // IO APIC的Global Int映射的中断向量号

// 终端时保存的上下文，必须和entries.asm中的代码一致
struct int_context {
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
typedef struct int_context int_context_t;

typedef void (*int_handler_t)(int vec, int_context_t *ctx);

// IDT
extern void idt_init();
extern void idt_load();
extern int_handler_t idt_get_int_handler(int vec);
extern void idt_set_int_handler(int vec, int_handler_t cb);

// PIC
extern void pic_init();
extern void pic_mask(int irq);
extern void pic_unmask(int irq);
extern void pic_send_eoi(int irq);

// IO APIC
extern int io_apic_count;
extern void io_apic_add(ACPI_MADT_IO_APIC *io_apic);
extern void io_apic_interrupt_override(ACPI_MADT_INTERRUPT_OVERRIDE *override);
extern void io_apic_init();
extern int io_apic_irq_to_gsi(int irq);
extern void io_apic_mask(int gsi);
extern void io_apic_unmask(int gsi);
static inline void io_apic_mask_irq(int irq) { io_apic_mask(io_apic_irq_to_gsi(irq)); }
static inline void io_apic_unmask_irq(int irq) { io_apic_unmask(io_apic_irq_to_gsi(irq)); }

// Local APIC
extern int local_apic_count;
extern void local_apic_add(ACPI_MADT_LOCAL_APIC *local_apic);
extern void local_apic_address_override(ACPI_MADT_LOCAL_APIC_OVERRIDE *override);
extern void local_apic_init();
extern void local_apic_send_eoi();

// local APIC Timer
extern void local_apic_timer_init();
extern void local_apic_timer_init_ap();
extern void local_apic_delay(int ticks);

#endif