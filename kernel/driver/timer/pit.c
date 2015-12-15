#include "pit.h"
#include "timer.h"
#include <common/util.h>
#include <interrupt/interrupt.h>

#define TIMER 1193180

#define CTRL_PORT 0x43
#define DATA_PORT 0x40

void pit_int_handler() {
    ++tick;

    // EOI
    out_byte(0x20, 0x20);
    io_wait();
}

void pit_init() {
    // install handler
    interrupt_handler_table[32] = pit_int_handler;

    out_byte(CTRL_PORT, 0x34);  // mode 2
    io_wait();

    out_byte(DATA_PORT, (TIMER/HZ));
    io_wait();
    out_byte(DATA_PORT, (TIMER/HZ) >> 8);
    io_wait();
}