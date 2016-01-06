#include "apic.h"
#include <timming/timming.h>
#include <utilities/env.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>

void local_apic_init(uint64_t base) {
    // query CPUID to see if it supports x2APIC
    // uint32_t a, b, c, d;
    // cpuid(1, &a, &b, &c, &d);

    // set the physical address for local APIC registers
    uint64_t apic_base_msr = read_msr(0x1b) & 0x0f00;   // only preserve flags
    apic_base_msr |= (uint64_t) base & (~0x0fffUL);     // rewrite the base addr
    // if (c & (1 << 21)) {        // enable x2APIC if it supports
    //     apic_base_msr |= 1UL << 10;
    // }
    apic_base_msr |= 1UL << 11; // enable this local APIC
    write_msr(0x1b, apic_base_msr);

    // set default APIC register values
    DATA_U32(base + TASK_PRIORITY)  = 0x20;

    // enable local apic
    // uint32_t spurious = * ((uint64_t *) base + SPURIOUS_INT_VECTOR);
    // * ((uint64_t *) base + SPURIOUS_INT_VECTOR) = spurious | 0x100;
    DATA_U32(base + SPURIOUS_INT_VECTOR) |= 1UL << 8;

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