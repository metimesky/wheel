#include <lib/locking.h>
#include <scheduler.h>

void spin_lock2(raw_spinlock_t *lock) {
    preempt_disable();
    while (!raw_spin_trylock(lock)) {
        preempt_enable();
    }
    preempt_enable();
}