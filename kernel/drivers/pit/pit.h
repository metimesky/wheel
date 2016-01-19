#ifndef __PIT_H__
#define __PIT_H__ 1

// i8253 PIT or compatible chip
// can also map to GSI, allow override

// 8253 PIT frequency

extern void pit_init();
extern void pit_map_gsi(int gsi);

#endif // __PIT_H__