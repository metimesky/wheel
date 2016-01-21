#include "io_apic.h"
#include <utilities/logging.h>
#include <drivers/pit/pit.h>

/* This file is not complete. we still lack configure of each GSI.
 * how to set vector number of each gsi, how to set polarity, trigger mode,
 * and target CPU of each GSI?
 */

/* We might have multiple IO APICs, each has a different GSI base and pin number,
 * So we have to manage that.
 * When initializing IO APIC, the tricky part is setting the trigger mode and
 * pin polarity:
 * - for IRQ (IO APIC reg 0~15), set to edge-trigger, active high
 * - for PCI A~D (IO APIC reg 16~19), set to level-trigger, active low
 */

// maximum number of IO APIC supported
#define MAX_IO_APIC_NUM 16

// IO APIC register address map
// referring to intel 82093AA datasheet
#define IO_REG_SELECT           0x00
#define IO_WINDOW               0x10

// the following IO APIC registers must be accessed
// using address in IO_REG_SELECT register
#define IO_APIC_ID              0x00
#define IO_APIC_VERSION         0x01
#define IO_APIC_ARBITRATION     0x02
#define IO_REDIRECT_TABLE_L(i)  (0x10 + 2 * (i))
#define IO_REDIRECT_TABLE_H(i)  (0x11 + 2 * (i))


// store these information globally
static int io_apic_count;
static ACPI_MADT_IO_APIC *io_apic_arr[MAX_IO_APIC_NUM];

// interrupt override (irq number is the index)
static uint32_t irq_target[16];
static uint16_t irq_flags[16];


// read register helper functions
static inline uint32_t io_apic_read(uint64_t base, uint32_t key) {
    DATA_U32(base + IO_REG_SELECT) = key;
    return DATA_U32(base + IO_WINDOW);
}

// write register helper functions
static inline void io_apic_write(uint64_t base, uint32_t key, uint32_t val) {
    DATA_U32(base + IO_REG_SELECT) = key;
    DATA_U32(base + IO_WINDOW) = val;
}

// get corresponding global system interrupt number from irq
static inline uint32_t irq_to_gsi(uint8_t irq) {
    if (irq < 16 && irq_target[irq] != 0xffffffff) {
        return irq_target[irq];
    } else {
        return irq;
    }
}

// this functions should be called before any IO APIC intialization
void io_apic_init() {
    io_apic_count = 0;
    for (int i = 0; i < MAX_IO_APIC_NUM; ++i) {
        io_apic_arr[i] = NULL;
    }
    for (int i = 0; i < 16; ++i) {
        irq_target[i] = 0xffffffff;
        irq_flags[i] = 0xffff;
    }
}

// add an instance of the IO APIC
void io_apic_add(ACPI_MADT_IO_APIC *ioapic) {
    if (io_apic_count >= MAX_IO_APIC_NUM) {
        // too much IO APIC, what a huge machine!!!
        return;
    }

    io_apic_arr[io_apic_count] = ioapic;
    ++io_apic_count;

    // now, start to initialize this IO APIC
    uint64_t base = ioapic->Address;

    uint32_t id = io_apic_read(base, IO_APIC_ID);
    uint32_t version = io_apic_read(base, IO_APIC_VERSION);
    int pin_count = (version >> 16) & 0xff;

    // initialize all input pins
    for (int i = 0; i < pin_count; ++i) {
        // uint32_t entry_low = 0;
        // ;
        io_apic_write(base, IO_REDIRECT_TABLE_L(i), 1U << 16);
    }
}

// redirect IRQ to global system interrupt
void io_apic_interrupt_override(ACPI_MADT_INTERRUPT_OVERRIDE *override) {
    uint8_t irq = override->SourceIrq;
    uint32_t gsi = override->GlobalIrq;

    log("int override irq:%d -> gsi:%d", irq, gsi);

    if (override->SourceIrq < 16) {
        irq_target[override->SourceIrq] = override->GlobalIrq;
        irq_flags[override->SourceIrq] = override->IntiFlags;
    }

    // // find the right IO APIC that contain that gsi
    // for (int i = 0; i < io_apic_count; ++i) {
    //     int pin_count = (io_apic_read(io_apic_arr[i]->Address, IO_APIC_VERSION) >> 16) & 0xff;
    //     if (io_apic_arr[i]->GlobalIrqBase <= gsi && gsi < io_apic_arr[i]->GlobalIrqBase + pin_count) {
    //         // then we found it
    //     }
    // }
}

void io_apic_set_gsi(uint32_t gsi, uint64_t ent) {
    // first find the right IO APIC that contain that gsi
    for (int i = 0; i < io_apic_count; ++i) {
        int pin_count = (io_apic_read(io_apic_arr[i]->Address, IO_APIC_VERSION) >> 16) & 0xff;
        if (io_apic_arr[i]->GlobalIrqBase <= gsi && gsi < io_apic_arr[i]->GlobalIrqBase + pin_count) {
            // then we mask the interrupt simple by setting bit 16 to one.
            uint32_t pin_index = gsi - io_apic_arr[i]->GlobalIrqBase;
            io_apic_write(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_L(pin_index), ent & 0xffffffff);
            io_apic_write(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_H(pin_index), (ent >> 32) & 0xffffffff);
        }
    }
}

// mask a global system interrupt
void io_apic_mask(uint32_t gsi) {
    // first find the right IO APIC that contain that gsi
    for (int i = 0; i < io_apic_count; ++i) {
        int pin_count = (io_apic_read(io_apic_arr[i]->Address, IO_APIC_VERSION) >> 16) & 0xff;
        if (io_apic_arr[i]->GlobalIrqBase <= gsi && gsi < io_apic_arr[i]->GlobalIrqBase + pin_count) {
            // then we mask the interrupt simple by setting bit 16 to one.
            uint32_t pin_index = gsi - io_apic_arr[i]->GlobalIrqBase;
            uint32_t entry_low = io_apic_read(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_L(pin_index));
            io_apic_write(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_L(pin_index), entry_low | (1U << 16));
        }
    }
}

// unmask a global system interrupt
void io_apic_unmask(uint32_t gsi) {
    // first find the right IO APIC that contain that gsi
    for (int i = 0; i < io_apic_count; ++i) {
        int pin_count = (io_apic_read(io_apic_arr[i]->Address, IO_APIC_VERSION) >> 16) & 0xff;
        if (io_apic_arr[i]->GlobalIrqBase <= gsi && gsi < io_apic_arr[i]->GlobalIrqBase + pin_count) {
            // then we unmask the interrupt by clearing bit 16.
            uint32_t pin_index = gsi - io_apic_arr[i]->GlobalIrqBase;
            uint32_t entry_low = io_apic_read(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_L(pin_index));
            io_apic_write(io_apic_arr[i]->Address, IO_REDIRECT_TABLE_L(pin_index), entry_low & ~(1U << 16));
        }
    }
}

// this function is just a temporary test function
void io_apic_magic() {
    // set GSI 2 to vector 88 (randomly chosen)
    io_apic_read(io_apic_arr[0]->Address, IO_REDIRECT_TABLE_L(2));
}