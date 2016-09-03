#ifndef __PIT_H__
#define __PIT_H__

extern void pit_init();
extern void pit_map_gsi(int vec);
extern void pit_delay(int ticks);

#endif