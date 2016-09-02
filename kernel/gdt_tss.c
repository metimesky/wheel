#include "gdt_tss.h"
#include <interrupt.h>
#include <percpu.h>
#include <lib/string.h>

#include <drivers/console.h>

extern uint64_t kernel_end_addr;

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;

static uint64_t *gdt;
static gdt_ptr_t gdtr;

DEFINE_PERCPU(tss_t*, tss);

// 该函数必须在per-CPU Data初始化之后调用
void __init gdt_init() {
    // 16字节对齐
    kernel_end_addr += 15UL;
    kernel_end_addr &= ~15UL;

    // 分配GDT占用的空间
    gdt = (uint64_t *)kernel_end_addr;
    int gdt_length = sizeof(uint64_t) * (5 + 2 * local_apic_count);
    kernel_end_addr += gdt_length + 15UL;
    kernel_end_addr &= ~15UL;

    gdt[0] = 0UL;                   // dummy descriptor
    gdt[1] = 0x00a0980000000000UL;  // kernel code segment
    gdt[2] = 0x00c0920000000000UL;  // kernel data segment
    gdt[3] = 0x00a0f80000000000UL;  // user code segment
    gdt[4] = 0x00c0f20000000000UL;  // user data segment

    // 循环填充TSS描述符
    for (int i = 0; i < local_apic_count; ++i) {
        // 分配TSS的空间
        PERCPU_ID(tss, i) = (tss_t *) kernel_end_addr;
        kernel_end_addr += sizeof(tss_t) + 15UL;
        kernel_end_addr &= ~15UL;

        uint64_t base = (uint64_t)PERCPU_ID(tss, i);
        uint64_t limit = sizeof(tss_t);
        console_print("TSS base %x, len %x\n", base, limit);
        
        // TSS清零
        //memset(base, 0, limit);

        gdt[2*i + 5] = 0;
        gdt[2*i + 5] &= 0x000000000000ffffUL & limit;           // limit [15:0]
        gdt[2*i + 5] &= 0x000000ffffff0000UL & (base << 16);    // base [23:0]
        gdt[2*i + 5] &= 0x0000e90000000000UL;                   // P, DPL=3, 64bit Available TSS
        gdt[2*i + 5] &= 0x000f000000000000UL & (limit << 32);   // limit [19:16]
        gdt[2*i + 5] &= 0xff00000000000000UL & (base << 32);    // base [31:24]

        gdt[2*i + 6]  = 0x00000000ffffffffUL & (base >> 32);
    }

    // 准备gdt_ptr
    gdtr.base = (uint64_t) gdt;
    gdtr.limit = gdt_length - 1;

    gdt_load();
}

extern void switch_gdt();
void __init gdt_load() {
    __asm__ __volatile__("lgdt (%0)" :: "a"(&gdtr));
    switch_gdt();
}

extern char kernel_stack_top;
void __init tss_init() {
    tss_t *tss = PERCPU(tss);
    memset(tss, 0, sizeof(tss_t));
    int i = read_gsbase();
    tss->rsp0 = (uint64_t)(&kernel_stack_top + __percpu_offset * i);

    tss_load();
}

void __init tss_load() {
    int cpu_id = read_gsbase();
    __asm__ __volatile__("ltr %%ax" :: "a"(((2*cpu_id + 5) << 3) + 3));
}