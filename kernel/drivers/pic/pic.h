#ifndef __PIC_H__
#define __PIC_H__ 1

#include <utilities/clib.h>

#define PIC1            0x20        /* IO base address for master PIC */
#define PIC2            0xA0        /* IO base address for slave PIC */
#define PIC1_CMD        PIC1
#define PIC1_DAT        (PIC1+1)
#define PIC2_CMD        PIC2
#define PIC2_DAT        (PIC2+1)

#define ICW1_ICW4       0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08        /* Level triggered (edge) mode */
#define ICW1_INIT       0x10        /* Initialization - required! */

#define ICW4_8086       0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C        /* Buffered mode/master */
#define ICW4_SFNM       0x10        /* Special fully nested (not) */

void pic_init();
void pic_mask(int line);
void pic_unmask(int line);

// send End-Of-Interrupt to PIC
#define pic_eoi(irq)                            \
    if ((irq) >= 8) {                           \
        out_byte(PIC2_CMD, 0x20);               \
        io_wait();                              \
    }                                           \
    out_byte(PIC1_CMD, 0x20);                   \
    io_wait();

#endif // __PIC_H__