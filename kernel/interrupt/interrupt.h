#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__ 1

#define INT_NUM 128
extern void* interrupt_handler_table[INT_NUM];

extern void interrupt_init();

#endif // __INTERRUPT_H__