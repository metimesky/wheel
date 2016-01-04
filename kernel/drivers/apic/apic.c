#include "apic.h"
#include <timming/timming.h>
#include <utilities/clib.h>
#include <utilities/cpu.h>

void io_apic_init(uint64_t base) {
    ;
}
/*
void local_apic_init(uint64_t base) {
    // set the physical address for local APIC registers
    uint64_t msr_apic_base = (uint64_t) base & (~0xfffUL);
    msr_apic_base |= 1UL << 8;  // this processor is BSP
    write_msr(0x1b, msr_apic_base);

    // enable local apic
    uint32_t spurious = * ((uint64_t *) base + SPURIOUS_INT_VECTOR);
    * ((uint64_t *) base + SPURIOUS_INT_VECTOR) = spurious | 0x100;

    // local APIC timer setup

    // block if a timer event is pending
    while (DATA_U32(base + LVT_TIMER) & (1U << 12)) {}

    // enable timer
    DATA_U32(base + LVT_TIMER) = 1UL << 16;

    DATA_U32(base + DIVIDE_CONFIGURATION) = 0;   // divide by 2
    DATA_U32(base + INITIAL_COUNT) = 0xffffffff; // maximum countdown

    uint64_t tick_a = tick;
    uint32_t count_a = DATA_U32(base + CURRENT_COUNT);

    while (tick < tick_a + 50) {}
    uint32_t count_b = DATA_U32(base + CURRENT_COUNT);

    // disable timer
    DATA_U32(base + LVT_TIMER) = 0UL;

    log("tick 50's differiential is %d.", count_a - count_b);
}
*/