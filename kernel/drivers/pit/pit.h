#ifndef __PIT_H__
#define __PIT_H__ 1

// i8253 PIT or compatible chip
// can also map to GSI, allow override

// 8253 PIT frequency
#define TIMER 1193180

#define CTRL_PORT 0x43
#define DATA_PORT 0x40

extern void pit_init();
extern void pit_map_gsi(int gsi);

#endif // __PIT_H__