#include "semaphore.h"
#include <memory/memory.h>

semaphore_t *semaphore_create() {
    semaphore_t *sem = (semaphore_t *) slab_alloc(sizeof(semaphore_t));
    sem->value = 1;
    sem->wait_queue = NULL;
    return sem;
}

void semaphore_destroy(semaphore_t *sem) {
    // wake all pending processes
    slab_free(sem);
}

void semaphore_wait(semaphore_t *sem) {
    ;
}

void semaphore_signal(semaphore_t *sem) {
    ;
}

////////////////////////////////////////////////////////////////////////////////
/// Atomic Primitives
////////////////////////////////////////////////////////////////////////////////

static inline void mb() {
    __asm __volatile("mfence;" : : : "memory");
}

static inline void wmb() {
    __asm __volatile("sfence;" : : : "memory");
}

static inline void rmb() {
    __asm __volatile("lfence;" : : : "memory");
}

typedef uint32_t spinlock_t;
#define SPINLOCK_FREE 0
#define SPINLOCK_BUSY 0xffffffff

spinlock_t *spinlock_create() {
    spinlock_t *lock = (spinlock_t *) slab_alloc(sizeof(spinlock_t));
    *lock = SPINLOCK_FREE;
    return lock;
}

void spinlock_destroy(spinlock_t *lock) {
    slab_free(lock);
}

void spinlock_lock(spinlock_t *lock) {
    while (!spinlock_trylock(lock)) {}
}

bool spinlock_trylock(spinlock_t *lock) {
    spinlock_t old = 0xffffffff;
    __asm__ __volatile__("lock xchgl %0, %1" : "=r"(old) : "m"(*lock), "0"(old) : "memory");
    return old == SPINLOCK_FREE;
}

void spinlock_unlock(spinlock_t *lock) {
    // TODO: should use atom primitives
    // *lock = SPINLOCK_FREE;
    __asm__ __volatile__("lock movl $0, %0" :: "m"(*lock));
}