#include "apic.h"

// IO APIC register address map
// referring to intel 82093AA datasheet
// io_apic_base is 0xfec0??00
#define IO_REGISTER_SELECT      0x00
#define IO_WINDOW               0x10

// the following IO APIC registers must be accessed
// using address in IO_REGISTER_SELECT register
#define IO_APIC_ID              0x00
#define IO_APIC_VERSION         0x01
#define IO_APIC_ARBITRATION     0x02
#define IO_REDIRECT_TABLE_0_L   0x10
#define IO_REDIRECT_TABLE_0_H   0x11
#define IO_REDIRECT_TABLE_1_L   0x12
#define IO_REDIRECT_TABLE_1_H   0x13
#define IO_REDIRECT_TABLE_2_L   0x14
#define IO_REDIRECT_TABLE_2_H   0x15
#define IO_REDIRECT_TABLE_3_L   0x16
#define IO_REDIRECT_TABLE_3_H   0x17
#define IO_REDIRECT_TABLE_4_L   0x18
#define IO_REDIRECT_TABLE_4_H   0x19
#define IO_REDIRECT_TABLE_5_L   0x1a
#define IO_REDIRECT_TABLE_5_H   0x1b
#define IO_REDIRECT_TABLE_6_L   0x1c
#define IO_REDIRECT_TABLE_6_H   0x1d
#define IO_REDIRECT_TABLE_7_L   0x1e
#define IO_REDIRECT_TABLE_7_H   0x1f
#define IO_REDIRECT_TABLE_8_L   0x20
#define IO_REDIRECT_TABLE_8_H   0x21
#define IO_REDIRECT_TABLE_9_L   0x22
#define IO_REDIRECT_TABLE_9_H   0x23
#define IO_REDIRECT_TABLE_10_L  0x24
#define IO_REDIRECT_TABLE_10_H  0x25
#define IO_REDIRECT_TABLE_11_L  0x26
#define IO_REDIRECT_TABLE_11_H  0x27
#define IO_REDIRECT_TABLE_12_L  0x28
#define IO_REDIRECT_TABLE_12_H  0x29
#define IO_REDIRECT_TABLE_13_L  0x2a
#define IO_REDIRECT_TABLE_13_H  0x2b
#define IO_REDIRECT_TABLE_14_L  0x2c
#define IO_REDIRECT_TABLE_14_H  0x2d
#define IO_REDIRECT_TABLE_15_L  0x2e
#define IO_REDIRECT_TABLE_15_H  0x2f
#define IO_REDIRECT_TABLE_16_L  0x30
#define IO_REDIRECT_TABLE_16_H  0x31
#define IO_REDIRECT_TABLE_17_L  0x32
#define IO_REDIRECT_TABLE_17_H  0x33
#define IO_REDIRECT_TABLE_18_L  0x34
#define IO_REDIRECT_TABLE_18_H  0x35
#define IO_REDIRECT_TABLE_19_L  0x36
#define IO_REDIRECT_TABLE_19_H  0x37
#define IO_REDIRECT_TABLE_20_L  0x38
#define IO_REDIRECT_TABLE_20_H  0x39
#define IO_REDIRECT_TABLE_21_L  0x3a
#define IO_REDIRECT_TABLE_21_H  0x3b
#define IO_REDIRECT_TABLE_22_L  0x3c
#define IO_REDIRECT_TABLE_22_H  0x3d
#define IO_REDIRECT_TABLE_23_L  0x3e
#define IO_REDIRECT_TABLE_23_H  0x3f

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
#define LVT_TERMAL_SENSOR       0x0330
#define LVT_PERF_MONIT_COUNTER  0x0340
#define LVT_LINT_0              0x0350
#define LVT_LINT_1              0x0360
#define LVT_ERROR               0x0370
#define INITIAL_COUNT           0x0380
#define CURRENT_COUNT           0x0390
#define DIVIDE_CONFIGURATION    0x03e0

void io_apic_init(uint64_t base) {
    ;
}

void local_apic_init(uint64_t base) {
    // enable local apic
    uint32_t spurious = * ((uint64_t *) base + SPURIOUS_INT_VECTOR);
    * ((uint64_t *) base + SPURIOUS_INT_VECTOR) = spurious | 0x100;
}