#include <wheel.h>
#include <interrupt.h>
#include <drivers/acpi/acpi.h>
#include <drivers/console.h>

// IO APIC register address map
// referring to intel 82093AA datasheet
#define IO_INDEX    0x00
#define IO_DATA     0x10
#define IO_IRQ_PIN  0x20    // IRQ pin assertion

// the following IO APIC registers must be accessed
// using address in IO_INDEX register
#define IO_APIC_ID      0x00
#define IO_APIC_VER     0x01
#define IO_APIC_ARB     0x02
#define IO_APIC_BOOT    0x03    // boot configuration
#define IO_REDTBL_L(i)  (0x10 + 2 * (i))
#define IO_REDTBL_H(i)  (0x11 + 2 * (i))

// redirection table entry upper 32bit
#define IO_APIC_DST     0xff000000

// redirection table entry lower 32 bit
#define IO_APIC_INT_MASK    0x00010000
#define IO_APIC_LEVEL       0x00008000
#define IO_APIC_EDGE        0x00000000
#define IO_APIC_REMOTE      0x00004000
#define IO_APIC_LOW         0x00002000
#define IO_APIC_HIGH        0x00000000
#define IO_APIC_LOGICAL     0x00000800
#define IO_APIC_PHYSICAL    0x00000000
#define IO_APIC_FIXED       0x00000000
#define IO_APIC_LOWEST      0x00000100
#define IO_APIC_SMI         0x00000200
#define IO_APIC_NMI         0x00000400
#define IO_APIC_INIT        0x00000500
#define IO_APIC_EXTINT      0x00000700
#define IO_APIC_VEC_MASK    0x000000ff

// read register helper functions
static inline uint32_t io_apic_read(uint64_t base, uint32_t key) {
    *(uint32_t *)(base + IO_INDEX) = key;
    return *(uint32_t *)(base + IO_DATA);
}

// write register helper functions
static inline void io_apic_write(uint64_t base, uint32_t key, uint32_t val) {
    *(uint32_t *)(base + IO_INDEX) = key;
    *(uint32_t *)(base + IO_DATA) = val;
}

struct io_apic {
    uint64_t base;
    int gsi_start;
    int gsi_count;
};
typedef struct io_apic io_apic_t;

// IO APIC ID只有4bit有效，因此IO APIC最多16个
static io_apic_t io_apic_list[16];
int io_apic_count;

// IRQ到GSI的映射初始均为-1
static int gsi_override[16] = { -1 };

// 该函数在BSP上执行，因此在找到IO APIC的同时就可以将其初始化
void io_apic_add(ACPI_MADT_IO_APIC *io_apic) {
    uint64_t base = KERNEL_VMA + io_apic->Address;

    uint32_t v = io_apic_read(base, IO_APIC_VER);
    int rednum = (v & 0x00ff0000) >> 16;

    // 将IO APIC ID设置成已知的值
    io_apic_write(base, IO_APIC_ID, io_apic_count);
    io_apic_list[io_apic_count].base = base;
    if (io_apic_count == 0) {
        io_apic_list[io_apic_count].gsi_start = 0;
    } else {
        io_apic_list[io_apic_count].gsi_start =
            io_apic_list[io_apic_count-1].gsi_start + io_apic_list[io_apic_count-1].gsi_count;
    }
    io_apic_list[io_apic_count].gsi_count = (v & 0x00ff0000) >> 16;
    ++io_apic_count;

    // 设置为front side bus
    // io_apic_write(base, IO_APIC_BOOT, 1);   // 0表示APIC serial bus, 1表示front side bus

    if (io_apic_count == 1) {
        uint32_t local_apic_id = 0;
        uint32_t upper32 = (local_apic_id << 24);
        uint32_t lower32;

        int i;
        for (i = 0; i < 16; ++i) {
            lower32 = IO_APIC_EDGE|IO_APIC_HIGH|IO_APIC_FIXED|IO_APIC_INT_MASK|IO_APIC_PHYSICAL; // |IO_APIC_INT_MASK
            lower32 |= GSI_VEC_BASE + i;
            io_apic_write(base, IO_REDTBL_H(i), upper32);
            io_apic_write(base, IO_REDTBL_L(i), lower32);
            console_print("red%d, %x %x\t", i, upper32, lower32);
        }
        console_print("\n");
        for (; i < rednum; ++i) {
            lower32 = IO_APIC_LEVEL|IO_APIC_LOW|IO_APIC_FIXED|IO_APIC_INT_MASK|IO_APIC_PHYSICAL; // |IO_APIC_INT_MASK
            lower32 |= GSI_VEC_BASE + i;
            io_apic_write(base, IO_REDTBL_H(i), upper32);
            io_apic_write(base, IO_REDTBL_L(i), lower32);
            console_print("red%d, %x %x\t", i, upper32, lower32);
        }
        console_print("\n");
    }
}

void io_apic_interrupt_override(ACPI_MADT_INTERRUPT_OVERRIDE *override) {
    // console_print("Override bus%d, irq%d to gsi%d\n", override->Bus, override->SourceIrq, override->GlobalIrq);
    int gsi = override->GlobalIrq;
    gsi_override[override->SourceIrq] = gsi;

    for (int i = 0; i < io_apic_count; ++i) {
        if (0 <= gsi && gsi < io_apic_list[i].gsi_count) {
            // console_print("masking index %d in IO APIC %d\n", gsi, i);
            uint32_t upper32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_H(gsi));
            uint32_t lower32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_L(gsi));
            io_apic_write(io_apic_list[i].base, IO_REDTBL_H(gsi), upper32);
            io_apic_write(io_apic_list[i].base, IO_REDTBL_L(gsi), lower32 | IO_APIC_INT_MASK);
            return;
        }
        gsi -= io_apic_list[i].gsi_count;
    }
}

void io_apic_init() {
    ;
}

// GSI means Global System Interrupt
// by default APIC maps 16 8259 irq to GSI 0~15, but that can be changed
int io_apic_irq_to_gsi(int irq) {
    if (gsi_override[irq] != -1) {
        return gsi_override[irq];
    } else {
        return irq;
    }
}

void io_apic_mask(int gsi) {
    // gsi -= GSI_VEC_BASE;
    for (int i = 0; i < io_apic_count; ++i) {
        if (0 <= gsi && gsi < io_apic_list[i].gsi_count) {
            // console_print("masking index %d in IO APIC %d\n", gsi, i);
            uint32_t upper32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_H(gsi));
            uint32_t lower32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_L(gsi));
            io_apic_write(io_apic_list[i].base, IO_REDTBL_H(gsi), upper32);
            io_apic_write(io_apic_list[i].base, IO_REDTBL_L(gsi), lower32 | IO_APIC_INT_MASK);
            return;
        }
        gsi -= io_apic_list[i].gsi_count;
    }
}

void io_apic_unmask(int gsi) {
    // gsi -= GSI_VEC_BASE;
    for (int i = 0; i < io_apic_count; ++i) {
        if (0 <= gsi && gsi < io_apic_list[i].gsi_count) {
            // console_print("unmasking index %d in IO APIC %d\n", gsi, i);
            uint32_t upper32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_H(gsi));
            uint32_t lower32 = io_apic_read(io_apic_list[i].base, IO_REDTBL_L(gsi));
            io_apic_write(io_apic_list[i].base, IO_REDTBL_H(gsi), upper32);
            io_apic_write(io_apic_list[i].base, IO_REDTBL_L(gsi), lower32 & (~IO_APIC_INT_MASK));
            return;
        }
        gsi -= io_apic_list[i].gsi_count;
    }
}