#include "pit.h"
#include <utilities/env.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>
#include <drivers/pic/pic.h>
#include <drivers/apic/apic.h>
#include <timing/timing.h>
#include <interrupt/interrupt.h>

/* PIT is an old timming device, the primary use of PIT is to synchronize
 * the more acurate APIC timer.
 */

char *video = (char *) 0xb8000;

static inline void real_handler() {
    ++video[0];
    ++tick;
}

static void pit_pic_irq_handler() {
    real_handler();
    pic_send_eoi(0);    // EOI
}

static void pit_apic_gsi_handler() {
    real_handler();
    // --video[158];
    local_apic_send_eoi();   // EOI
}

void pit_init() {
    // install handler
    interrupt_install_handler(PIC_IRQ_VEC_BASE + 0, pit_pic_irq_handler);

    out_byte(CTRL_PORT, 0x34);  // mode 2
    io_wait();

    out_byte(DATA_PORT, (TIMER/HZ));
    io_wait();
    out_byte(DATA_PORT, (TIMER/HZ) >> 8);
    io_wait();
}

// GSI means Global System Interrupt
// by default APIC maps 16 8259 irq to GSI 0~15, but that can be changed

void pit_map_gsi(int gsi) {
    log("mapping pit to gsi");
    interrupt_install_handler(gsi, pit_apic_gsi_handler);
}