#ifndef __PIC_H__
#define __PIC_H__

extern void pic_init();
extern void pic_mask(int irq);
extern void pic_unmask(int irq);

#endif