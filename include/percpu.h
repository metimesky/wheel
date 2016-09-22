#ifndef __PERCPU_H__
#define __PERCPU_H__

#include <wheel.h>
#include <scheduler.h>
#include <lib/cpu.h>

// 使用per-CPU Data可以省去加锁的必要，但所有per-CPU Data仍处在同一地址空间中
// 只能创建基本类型和结构体，无法创建数组

// 定义per-CPU Data的宏
#define DEFINE_PERCPU_VAL(type, name, value) \
    __attribute__((section(".percpu.data"))) type __percpu_##name = (value)

#define DEFINE_PERCPU(type, name) \
    __attribute__((section(".percpu.bss"))) type __percpu_##name

#define EXPORT_PERCPU(type, name) \
    extern DEFINE_PERCPU(type, name)

// __percpu_offset是相邻两段per-CPU Data之间的偏移，CPU的索引保存在GS.BASE中
extern uint64_t __percpu_offset;

// 访问per-CPU Data的宏
// 访问per-CPU var的过程中，因此最好首先禁用抢占
#define raw_percpu_ptr_id(name, id) \
    ((char*)&__percpu_##name + __percpu_offset*id)
#define raw_percpu_ptr(name) \
    ((char*)&__percpu_##name + __percpu_offset*read_gsbase())

#define get_cpu_var(name)                   \
(*({                                        \
    preempt_disable();                      \
    raw_percpu_ptr(name);                   \
}))

#define get_cpu_var_id(name, id)            \
(*({                                        \
    preempt_disable();                      \
    raw_percpu_ptr_id(name, id);            \
}))

#define put_cpu_var(name)                   \
({                                          \
    preempt_enable();                       \
})

#endif