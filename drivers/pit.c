#include <drivers/pit.h>
#include <interrupt.h>
#include <lib/cpu.h>

/* PIT is an old timming device, when HPET is not available, we use PIT to
 * achieve acurate timing.
 * This file assumes a 8254 chip.
 */

// 应该定义通用的timer类，与具体实现无关，里面有sysClockInt和auxClockInt函数，
// 由相应的设备在真正的中断处理函数中调用

#define TIMER 1193182
#define HZ 1000

#define CTRL_PORT 0x43
#define DATA_PORT 0x40

char *video = (char *)(KERNEL_VMA + 0xa0000);
static volatile uint64_t pit_tick = 0;

static inline void real_handler() {
    ++pit_tick;
    if (pit_tick % 100 == 0) {
        // pit_tick = 0;
        ++video[158];
    }
}

static void pit_pic_handler(int vec, int_context_t *ctx) {
    real_handler();
    pic_send_eoi(0);    // EOI
}

static void pit_apic_handler(int vec, int_context_t *ctx) {
    real_handler();
    local_apic_send_eoi();   // EOI
}

void pit_delay(int ticks) {
    int end_tick = pit_tick + ticks;
    while (pit_tick < end_tick) {
        // video[4] ++;
    }
}

void __init pit_init() {
    // install both IRQ and GSI handler
    idt_set_int_handler(IRQ_VEC_BASE + 0, pit_pic_handler);
    idt_set_int_handler(GSI_VEC_BASE + io_apic_irq_to_gsi(0), pit_apic_handler);

    // 00110110b = 32+16+4+2 = 0x36
    out_byte(CTRL_PORT, 0x36);  // mode 2, square wave
    io_wait();

    // out_byte(DATA_PORT, (TIMER/HZ));
    out_byte(DATA_PORT, 11932U & 0xff);
    io_wait();
    // out_byte(DATA_PORT, (TIMER/HZ) >> 8);
    out_byte(DATA_PORT, (11932U >> 8) & 0xff);
    io_wait();
}
