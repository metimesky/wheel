// 8042 PS/2 Keyboard driver

#include <drivers/kb.h>
#include <interrupt.h>
#include <lib/cpu.h>
#include <drivers/console.h>

#define KB_DATA 0x60
#define KB_STAT 0x64    // read only
#define KB_CMD  0x64    // write only

static inline void real_handler() {
    uint8_t scan = in_byte(0x60);
    if (scan & 0x80) {
        // key release event
        console_print("^");
    } else {
        // key press event
        console_print("v");
    }
}

static void kb_pic_handler() {
    real_handler();
    pic_send_eoi(1);
}

static void kb_apic_handler() {
    real_handler();
    local_apic_send_eoi();
}

void __init kb_init() {
    idt_set_int_handler(IRQ_VEC_BASE + 1, kb_pic_handler);
    idt_set_int_handler(GSI_VEC_BASE + io_apic_irq_to_gsi(1), kb_apic_handler);

    // 检查ACPI::FADT->IA_PC_Boot_Architecture_Flags bit 1，判断键盘设备是否存在
    // 如果这个位是1，则存在键盘
if (0) {
    out_byte(KB_CMD, 0xad);
    while (in_byte(KB_STAT) & 2) { }

    out_byte(KB_CMD, 0xa7);
    while (in_byte(KB_STAT) & 2) { }
}
    // flush output buffer
    in_byte(KB_DATA);
if (0) {
    // controller configuration byte
    out_byte(KB_CMD, 0x20);
    uint8_t conf = in_byte(KB_DATA);

    conf &= ~(1U << 0);
    conf &= ~(1U << 1);
    conf &= ~(1U << 6);

    out_byte(KB_CMD, 0x60);
    out_byte(KB_DATA, conf);
}
    // reset LED lights
    out_byte(KB_CMD, 0xed);
    out_byte(KB_CMD, 0);
}