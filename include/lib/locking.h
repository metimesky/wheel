#ifndef __LOCKING_H__
#define __LOCKING_H__

#include <wheel.h>

// 原子操作

static inline int atomic_swap(volatile int *x, int v) {
    __asm__ __volatile__("xchg %0, %1" : "=r"(v), "=m"(*x) : "0"(v) : "memory");
    return v;
}

static inline void atomic_store(volatile int *p, int x) {
    __asm__ __volatile__("movl %1, %0" : "=m"(*p) : "r"(x) : "memory");
}

static inline void atomic_inc(volatile int *x) {
    __asm__ __volatile__("lock; incl %0" : "=m"(*x) : "m"(*x) : "memory");
}

static inline void atomic_dec(volatile int *x) {
    __asm__ __volatile__("lock; decl %0" : "=m"(*x) : "m"(*x) : "memory");
}

// 自旋锁，可以实现多核之间的互斥，但未考虑抢占
typedef volatile uint32_t raw_spinlock_t;
extern void raw_spin_lock(raw_spinlock_t *lock);
extern bool raw_spin_trylock(raw_spinlock_t *lock);
extern void raw_spin_unlock(raw_spinlock_t *lock);      // 该函数不会检查是否已上锁
extern bool raw_spin_islocked(raw_spinlock_t *lock);

// 真正的自旋锁

#endif