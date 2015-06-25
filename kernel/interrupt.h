#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__ 1

extern void interrupt_init();

#define INT_NUM 128
extern void* interrupt_handler[INT_NUM];

#endif // __INTERRUPT_H__