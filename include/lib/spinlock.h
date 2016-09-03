#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <wheel.h>

typedef volatile uint32_t spinlock_t;

extern void spinlock_lock(spinlock_t *lock);
extern void spinlock_free(spinlock_t *lock);

#endif