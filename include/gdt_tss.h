#ifndef __GDT_TSS_H__
#define __GDT_TSS_H__

#include <wheel.h>
#include <percpu.h>

// TSS中保存不同特权级的rsp，带特权级切换的中断需要TSS
struct tss {
    uint32_t reserved_1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_3;
    uint16_t reserved_4;
    uint16_t io_map_base;
} __attribute__((packed));
typedef struct tss tss_t;

EXPORT_PERCPU(tss_t, tss);

extern void gdt_init();
extern void gdt_load();

extern void tss_init();
extern void tss_load();

#endif