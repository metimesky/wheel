#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <wheel.h>

extern void create_process(uint64_t entry);
extern void create_process3(uint64_t entry);

extern void start_schedule();

extern void preempt_enable();
extern void preempt_disable();

#endif