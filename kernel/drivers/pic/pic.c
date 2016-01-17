#include "pic.h"
#include <utilities/cpu.h>

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

// initialize dual 8259 PICs, all pins are disabled by default.
// If the system use APIC, then PIC should be disabled;
// If the system use PIC, then each pin can be enabled later.
void pic_init() {
    out_byte(PIC1_CMD, ICW1_INIT+ICW1_ICW4);    // starts the initialization sequence (in cascade mode)
    io_wait();
    out_byte(PIC2_CMD, ICW1_INIT+ICW1_ICW4);
    io_wait();
    out_byte(PIC1_DAT, PIC_IRQ_VEC_BASE);       // ICW2: map master PIC vector base
    io_wait();
    out_byte(PIC2_DAT, PIC_IRQ_VEC_BASE + 8);   // ICW2: map slave PIC vector base
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
void pic_mask(uint8_t irq) {
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
void pic_unmask(uint8_t irq) {
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

void pic_send_eoi(uint8_t irq) {
    if ((irq) >= 8) {
        out_byte(PIC2_CMD, 0x20);
        io_wait();
    }
    out_byte(PIC1_CMD, 0x20);
    io_wait();
}