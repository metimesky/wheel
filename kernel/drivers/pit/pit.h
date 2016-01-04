#ifndef __PIT_H__
#define __PIT_H__ 1

// 8253 PIT frequency
#define TIMER 1193180

#define CTRL_PORT 0x43
#define DATA_PORT 0x40

extern void pit_init();

#endif // __PIT_H__