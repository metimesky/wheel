#include "apic.h"
#include <timming/timming.h>
#include <utilities/env.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>
#include <drivers/pic/pic.h>

// local APIC register address map
// local_apic_base is 0xfee00000
#define LOCAL_APIC_ID           0x0020
#define LOCAL_APIC_VERSION      0x0030
#define TASK_PRIORITY           0x0080
#define ARBITRATION_PRIORITY    0x0090
#define PROCESSOR_PRIORITY      0x00a0
#define EOI                     0x00b0
#define REMOTE_READ             0x00c0
#define LOGICAL_DESTINATION     0x00d0
#define DESTINATION_FORMAT      0x00e0
#define SPURIOUS_INT_VECTOR     0x00f0
#define IN_SERVICE_0            0x0100
#define IN_SERVICE_1            0x0110
#define IN_SERVICE_2            0x0120
#define IN_SERVICE_3            0x0130
#define IN_SERVICE_4            0x0140
#define IN_SERVICE_5            0x0150
#define IN_SERVICE_6            0x0160
#define IN_SERVICE_7            0x0170
#define TRIGGER_MODE_0          0x0180
#define TRIGGER_MODE_1          0x0190
#define TRIGGER_MODE_2          0x01a0
#define TRIGGER_MODE_3          0x01b0
#define TRIGGER_MODE_4          0x01c0
#define TRIGGER_MODE_5          0x01d0
#define TRIGGER_MODE_6          0x01e0
#define TRIGGER_MODE_7          0x01f0
#define INTERRUPT_REQUEST_0     0x0200
#define INTERRUPT_REQUEST_1     0x0210
#define INTERRUPT_REQUEST_2     0x0220
#define INTERRUPT_REQUEST_3     0x0230
#define INTERRUPT_REQUEST_4     0x0240
#define INTERRUPT_REQUEST_5     0x0250
#define INTERRUPT_REQUEST_6     0x0260
#define INTERRUPT_REQUEST_7     0x0270
#define ERROR_STATUS            0x0280
#define LVT_CMCI                0x02f0
#define INTERRUPT_COMMAND_0     0x0300
#define INTERRUPT_COMMAND_1     0x0310
#define LVT_TIMER               0x0320
#define LVT_THERMAL_SENSOR      0x0330
#define LVT_PERF_MONIT_COUNTER  0x0340
#define LVT_LINT_0              0x0350
#define LVT_LINT_1              0x0360
#define LVT_ERROR               0x0370
#define INITIAL_COUNT           0x0380
#define CURRENT_COUNT           0x0390
#define DIVIDE_CONFIGURATION    0x03e0

void local_apic_init(uint64_t base) {
    // query CPUID to see if it supports x2APIC
    // uint32_t a, b, c, d;
    // cpuid(1, &a, &b, &c, &d);

    // set the physical address for local APIC registers
    uint64_t apic_base_msr = read_msr(0x1b) & 0x0f00;   // only preserve flags
    apic_base_msr |= 1UL << 8;      // enable this local apic
    apic_base_msr |= (uint64_t) base & (~0x0fffUL);     // rewrite the base addr
    apic_base_msr |= 1UL << 11; // enable this local APIC
    write_msr(0x1b, apic_base_msr);

    // set default APIC register values
    DATA_U32(base + TASK_PRIORITY)  = 0x20;

    // enable local APIC
    DATA_U32(base + SPURIOUS_INT_VECTOR) |= 1UL << 8;

    // LVT - local APIC timer
    uint32_t lvt_0 = 0;
    lvt_0 |= 48 & 0x000000ff;   // vector = 48
    lvt_0 |= 0  & 0x00000700;   // message type = fixed
    lvt_0 |= 1 << 16;           // mask, unmask it later
    lvt_0 |= 1 << 17;           // timer mode = periodic
    DATA_U32(base + LVT_TIMER) = lvt_0;

    // LVT - thermal sensor
    uint32_t lvt_1 = 0;
    lvt_1 |= 49 & 0x000000ff;   // vector = 49
    lvt_1 |= 0  & 0x00000700;   // message type = fixed
    lvt_1 |= 1 << 16;           // mask, unmask it later
    DATA_U32(base + LVT_THERMAL_SENSOR) = lvt_1;

    // LVT - performance monitor counter
    uint32_t lvt_2 = 0;
    lvt_2 |= 50 & 0x000000ff;   // vector = 50
    lvt_2 |= 0  & 0x00000700;   // message type = fixed
    lvt_2 |= 1 << 16;           // mask, unmask it later
    DATA_U32(base + LVT_PERF_MONIT_COUNTER) = lvt_2;

    // if i init LINT0  now, the i8253 PIT clock will stop working
    // because LINT0 is also INTR, the incomming from PIC

    // // LVT - local interrupt 0 (LINT0)
    // uint32_t lvt_3 = 0;
    // lvt_3 |= 51 & 0x000000ff;   // vector = 51
    // lvt_3 |= 0  & 0x00000700;   // message type = fixed
    // lvt_3 |= 1 << 16;
    // DATA_U32(base + LVT_LINT_0) = lvt_3;

    // LVT - local interrupt 1 (LINT1)
    uint32_t lvt_4 = 0;
    lvt_4 |= 52 & 0x000000ff;   // vector = 52
    lvt_4 |= 0  & 0x00000700;   // message type = fixed
    lvt_4 |= 1 << 16;
    DATA_U32(base + LVT_LINT_1) = lvt_4;

////////////////////////////////////////////////////////////////////////////////
    // setting up APIC timer

    // block if a timer event is pending
    while (DATA_U32(base + LVT_TIMER) & (1U << 12)) {}

    DATA_U32(base + DIVIDE_CONFIGURATION) = 0;   // divide by 2
    DATA_U32(base + INITIAL_COUNT) = 0xffffffff; // maximum countdown

    // start timer (temperarily)
    DATA_U32(base + LVT_TIMER) &= ~(1 << 16);
    pic_unmask(0);

    uint64_t tick_a = tick;
    uint32_t count_a = DATA_U32(base + CURRENT_COUNT);

    while (tick < tick_a + 400) {}
    uint32_t count_b = DATA_U32(base + CURRENT_COUNT);

    // then disable timer again
    DATA_U32(base + LVT_TIMER) |= 1 << 16;
    pic_mask(0);

    log("tick 400's differiential is %d.", count_a - count_b);
}