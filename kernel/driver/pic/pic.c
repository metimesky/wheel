#include "pic.h"
#include <common/util.h>

void pic_init() {
    out_byte(PIC1_CMD, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    out_byte(PIC2_CMD, ICW1_INIT+ICW1_ICW4);
    io_wait();
    out_byte(PIC1_DAT, 32);     // ICW2: Master PIC vector offset
    io_wait();
    out_byte(PIC2_DAT, 40);     // ICW2: Slave PIC vector offset
    io_wait();
    out_byte(PIC1_DAT, 4);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    out_byte(PIC2_DAT, 2);      // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out_byte(PIC1_DAT, ICW4_8086);
    io_wait();
    out_byte(PIC2_DAT, ICW4_8086);
    io_wait();

    out_byte(PIC1_DAT, 0xff);   // mask all interrupts on master chip
    io_wait();
    out_byte(PIC2_DAT, 0xff);   // mask all interrupts on slave chip
    io_wait();
}

// mask an input of PIC to disable it
void pic_mask(int irq) {
    uint16_t port;
    uint8_t value;
 
    if (irq < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        irq -= 8;
    }

    value = in_byte(port) | (1 << irq);
    io_wait();

    out_byte(port, value);
    io_wait();
}

// clear an input of PIC to enable it
void pic_unmask(int irq) {
    uint16_t port;
    uint8_t value;
 
    if (irq < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        irq -= 8;
    }

    value = in_byte(port) & ~(1 << irq);
    io_wait();

    out_byte(port, value);
    io_wait();
}