#include <drivers/pit.h>
#include <interrupt.h>
#include <lib/cpu.h>

/* PIT is an old timming device, when HPET is not available, we use PIT to
 * achieve acurate timing.
 * This file assumes a 8254 chip.
 */

#define TIMER 1193180
#define HZ 1000

#define CTRL_PORT 0x43
#define DATA_PORT 0x40

char *video = (char *)(KERNEL_VMA + 0xa0000);
uint64_t tick = 0;

static inline void real_handler() {
    ++video[158];
    ++tick;
}

static void pit_pic_handler(int vec, interrupt_context_t *ctx) {
    real_handler();
    pic_send_eoi(0);    // EOI
}

static void pit_apic_handler(int vec, interrupt_context_t *ctx) {
    real_handler();
    local_apic_send_eoi();   // EOI
}

void pit_init() {
    // install handler
    interrupt_set_handler(IRQ_VEC_BASE + 0, pit_pic_handler);

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
    // log("mapping pit to gsi");
    interrupt_set_handler(gsi, pit_apic_handler);
}