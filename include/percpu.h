#ifndef __PERCPU_H__
#define __PERCPU_H__

#include <wheel.h>
#include <scheduler.h>
#include <lib/cpu.h>

// 使用per-CPU Data可以省去加锁的必要，但所有per-CPU Data仍处在同一地址空间中

// 定义per-CPU Data的宏
#define DEFINE_PERCPU_VAL(type, name, value) \
    __attribute__((section(".percpu.data"))) type __percpu_##name = (value)
#define DEFINE_PERCPU(type, name) \
    __attribute__((section(".percpu.bss"))) type __percpu_##name

// 访问per-CPU Data的宏
// __percpu_offset是相邻两段per-CPU Data之间的偏移，CPU的索引保存在GS.BASE中
// 访问per-CPU var的过程中，进程不能迁移到其他CPU上，因此需要首先禁用抢占
extern uint64_t __percpu_offset;

#define raw_percpu_id(name, id) \
    (*(unsigned int *)((char*)&__percpu_##name + __percpu_offset*id))
    // (*(__typeof__(&__percpu_##name))((char*)&__percpu_##name + __percpu_offset*id))
#define raw_percpu(name) \
    (*(unsigned int *)((char*)&__percpu_##name + __percpu_offset*read_gsbase()))
    // (*(__typeof__(&__percpu_##name))((char*)&__percpu_##name + __percpu_offset*read_gsbase()))

#define get_cpu_var(name)                   \
(*({                                        \
    preempt_disable();                      \
    raw_percpu(name);                       \
}))

#define get_cpu_var_id(name, id)            \
(*({                                        \
    preempt_disable();                      \
    raw_percpu_id(name, id);                \
}))

#define put_cpu_var(name)                   \
(*({                                        \
    preempt_enable();                       \
}))

#endif