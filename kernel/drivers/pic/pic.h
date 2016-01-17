#ifndef __PIC_H__
#define __PIC_H__ 1

#include <utilities/env.h>

// which interrupt vector does PIC IRQ map to
#define PIC_IRQ_VEC_BASE 32

void pic_init();
void pic_mask(uint8_t irq);
void pic_unmask(uint8_t irq);
void pic_send_eoi(uint8_t irq);

#endif // __PIC_H__