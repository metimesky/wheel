#ifndef __TIMMING_H__
#define __TIMMING_H__ 1

#include <utilities/env.h>

#define HZ 1000

extern volatile uint64_t tick;

void busy_wait(int msec);

#endif // __TIMMING_H__