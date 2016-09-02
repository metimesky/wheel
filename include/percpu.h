#ifndef __PERCPU_H__
#define __PERCPU_H__

#include <wheel.h>
#include <lib/cpu.h>

// 使用per-CPU Data可以省去加锁的必要，但所有per-CPU Data仍处在同一地址空间中

// 定义per-CPU Data的宏
#define DEFINE_PERCPU_VAL(type, name, value) \
    __attribute__((section(".percpu.data"))) __typeof__(type) __percpu_##name = (value)
#define DEFINE_PERCPU(type, name) \
    __attribute__((section(".percpu.bss"))) __typeof__(type) __percpu_##name

// 访问per-CPU Data的宏
// __percpu_offset是相邻两段per-CPU Data之间的偏移，CPU的索引保存在GS.BASE中
extern uint64_t __percpu_offset;
#define PERCPU_ID(name, id) \
    (*(unsigned long *)((char*)__percpu_##name + __percpu_offset*id))
#define PERCPU(name) \
    (*(unsigned long *)((char*)__percpu_##name + __percpu_offset*read_gsbase()))

#endif