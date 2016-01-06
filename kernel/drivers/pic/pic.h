#ifndef __PIC_H__
#define __PIC_H__ 1

#include <utilities/env.h>

void pic_init();
void pic_mask(int irq);
void pic_unmask(int irq);
void pic_send_eoi(int irq);

#endif // __PIC_H__