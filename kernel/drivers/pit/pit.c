#include "pit.h"
#include <utilities/clib.h>
#include <utilities/cpu.h>
#include <drivers/pic/pic.h>
#include <timming/timming.h>
#include <interrupt/interrupt.h>

void pic_pit_handler() {
    ++tick;
    pic_eoi(0); // EOI
}

void apic_pit_handler() {
    ++tick;
    // api2c_eoi(); // EOI
}

void pit_init() {
    // install handler
    interrupt_handler_table[32] = pic_pit_handler;
    // interrupt_handler_table[48] = apic_pit_handler;

    out_byte(CTRL_PORT, 0x34);  // mode 2
    io_wait();

    out_byte(DATA_PORT, (TIMER/HZ));
    io_wait();
    out_byte(DATA_PORT, (TIMER/HZ) >> 8);
    io_wait();
}