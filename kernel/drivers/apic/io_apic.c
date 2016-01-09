#include "io_apic.h"

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

void io_apic_init(uint64_t base) {
    ;
}
/*
int smp_irq_to_pin(unsigned int irq) {
    // ASSERT(ops != NULL);
    return ops->irq_to_pin(irq);
}

void io_apic_disable_irqs(uint16_t irqmask) {
    unsigned int i;
    for (i = 0; i < 16; i++) {
        if (irqmask & (1 << i)) {
            // Mask the signal input in IO APIC if there is a mapping for the respective IRQ number.
            int pin = smp_irq_to_pin(i);
            if (pin != -1) {
                io_redirection_reg_t reg;
                
                reg.lo = io_apic_read((uint8_t) (IOREDTBL + pin * 2));
                reg.masked = true;
                io_apic_write((uint8_t) (IOREDTBL + pin * 2), reg.lo);
            }
            
        }
    }
}*/