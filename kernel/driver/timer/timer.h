#ifndef __TIMER_H__
#define __TIMER_H__ 1

#include <common/stdhdr.h>

#define HZ 1000

extern volatile uint64_t tick;

void busy_wait(int msec);

#endif // __TIMER_H__