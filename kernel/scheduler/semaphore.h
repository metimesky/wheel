#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__ 1

#include <utilities/env.h>

struct semaphore {
    int value;
    void *wait_queue;
} __attribute__((packed));
typedef struct semaphore semaphore_t;

extern semaphore_t *semaphore_create();
extern void semaphore_destroy(semaphore_t *sem);
extern void semaphore_wait(semaphore_t *sem);
extern void semaphore_signal(semaphore_t *sem);

////////////////////////////////////////////////////////////////////////////////

typedef uint32_t spinlock_t;

extern spinlock_t *spinlock_create();
extern void spinlock_destroy(spinlock_t *lock);
extern void spinlock_lock(spinlock_t *lock);
extern bool spinlock_trylock(spinlock_t *lock);
extern void spinlock_unlock(spinlock_t *lock);

#endif // __SEMAPHORE_H__