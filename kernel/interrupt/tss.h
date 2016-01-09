#ifndef __TSS_H__
#define __TSS_H__ 1

#include <utilities/env.h>

/* Notice that in SMP environment, each CPU should has its own TSS.
 * But currently, Wheel has only one TSS because the space reserved
 * in boot.asm.
 * Multiple TSS will be considered in the future.
 */

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

////////////////////////////////////////////////////////////////////////////////

extern void tss_init();

// this function is needed when switching context. the kernel stack of target
// process should be specified in TSS
extern void tss_set_rsp0(uint64_t rsp);

#endif // __TSS_H__