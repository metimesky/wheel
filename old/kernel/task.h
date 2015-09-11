#ifndef __TASK_H__
#define __TASK_H__ 1

struct tss {
    uint32_t reserved_1;
    // uint32_t rsp0_lower;
    // uint32_t rsp0_upper;
    uint64_t rsp0;
    // uint32_t rsp1_lower;
    // uint32_t rsp1_upper;
    uint64_t rsp1;
    // uint32_t rsp2_lower;
    // uint32_t rsp2_upper;
    uint64_t rsp2;
    uint64_t reserved_2;
    // uint32_t ist1_lower;
    // uint32_t ist1_upper;
    uint64_t ist1;
    // uint32_t ist2_lower;
    // uint32_t ist2_upper;
    uint64_t ist2;
    // uint32_t ist3_lower;
    // uint32_t ist3_upper;
    uint64_t ist3;
    // uint32_t ist4_lower;
    // uint32_t ist4_upper;
    uint64_t ist4;
    // uint32_t ist5_lower;
    // uint32_t ist5_upper;
    uint64_t ist5;
    // uint32_t ist6_lower;
    // uint32_t ist6_upper;
    uint64_t ist6;
    // uint32_t ist7_lower;
    // uint32_t ist7_upper;
    uint64_t ist7;
    uint64_t reserved_3;
    uint16_t reserved_4;
    uint16_t iomap_base;
} __attribute__((packed));
typedef struct tss tss_t;

#endif // __TASK_H__