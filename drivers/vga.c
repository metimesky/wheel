#include <drivers/vga.h>
#include <lib/cpu.h>

// VGA Miscellaneous Register
#define VGA_MISC_R 0x03cc
#define VGA_MISC_W 0x03c2

// VGA Sequencer Registers
#define VGA_SEQ_ADDR 0x03c4
#define VGA_SEQ_DATA 0x03c5

// VGA CRT Controller Registers
#define VGA_CRTC_ADDR 0x03d4
#define VGA_CRTC_DATA 0x03d5
#define VGA_MONO_CRTC_ADDR 0x03b4
#define VGA_MONO_CRTC_DATA 0x03b5

// VGA Graphics Controller Registers
#define VGA_GC_ADDR 0x03ce
#define VGA_GC_DATA 0x03cf

// VGA Attribute Controller Registers
#define	VGA_AC_ADDR   0x03c0
#define	VGA_AC_DATA_W 0x03c0
#define	VGA_AC_DATA_R 0x03c1

#define	VGA_INSTAT_R 0x03da

// set the display to 80x25 text mode, mapping frame buffer to 128K region in
// host memory starting from 0xa0000.
uint8_t regs_80x25_text[] = {
    // miscellaneous register
    0x67,
    // sequencer registers
    0x03, 0x00, 0x03, 0x00, 0x02,
    // CRT controller registers
    0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f, 0x00, 0x4f,
    0x0d, 0x0e, 0x00, 0x00, 0x00, 0x50, 0x9c, 0x0e, 0x8f, 0x28,
    0x1f, 0x96, 0xb9, 0xa3, 0xff,
    // graphics controller registers
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xff,
    // attribute controller registers
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39,
    0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x0c, 0x00, 0x0f, 0x08, 0x00
};

// set the display to 90x30 text mode, mapping frame buffer to 128K region in
// host memory starting from 0xa0000.
uint8_t regs_90x30_text[] = {
    // miscellaneous register
    0xe7,
    // sequencer registers
    0x03, 0x01, 0x03, 0x00, 0x02,
    // CRT controller registers
    0x6b, 0x59, 0x5a, 0x82, 0x60, 0x8d, 0x0b, 0x3e, 0x00, 0x4f,
    0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00, 0xea, 0x0c, 0xdf, 0x2d,
    0x10, 0xe8, 0x05, 0xa3, 0xff,
    // graphics controller registers
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xff,
    // attribute controller registers
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39,
    0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x0c, 0x00, 0x0f, 0x08, 0x00
};

// read the values of vga registers and store them in array
void vga_read_regs(uint8_t *regs) {
    // read miscellaneous registers
    *regs = in_byte(VGA_MISC_R);
    ++regs;

    // read sequencer registers
    for (int i = 0; i < 5; ++i) {
        out_byte(VGA_SEQ_ADDR, i);
        *regs = in_byte(VGA_SEQ_DATA);
        ++regs;
    }
    // read CRT controller registers
    for (int i = 0; i < 25; ++i) {
        out_byte(VGA_CRTC_ADDR, i);
        *regs = in_byte(VGA_CRTC_DATA);
        ++regs;
    }
    // read graphics controller registers
    for (int i = 0; i < 9; ++i) {
        out_byte(VGA_GC_ADDR, i);
        *regs = in_byte(VGA_GC_DATA);
        ++regs;
    }
    // read ATTRIBUTE CONTROLLER regs
    for (int i = 0; i < 21; ++i) {
        in_byte(VGA_INSTAT_R);
        out_byte(VGA_AC_ADDR, i);
        *regs = in_byte(VGA_AC_DATA_R);
        ++regs;
    }

    // lock 16-color palette and unblank display
    in_byte(VGA_INSTAT_R);
    out_byte(VGA_AC_ADDR, 0x20);
}

// write values from array to VGA registers
void vga_write_regs(uint8_t *regs) {
    // write miscellaneous output register
    *regs |= 1; // compatible for color monitor
    out_byte(VGA_MISC_W, *regs);
    ++regs;

    // write sequencer registers (5)
    for (int i = 0; i < 5; ++i) {
        out_byte(VGA_SEQ_ADDR, i);
        out_byte(VGA_SEQ_DATA, *regs);
        ++regs;
    }

    // unlock CRTC registers
    out_byte(VGA_CRTC_ADDR, 0x03);
    out_byte(VGA_CRTC_DATA, in_byte(VGA_CRTC_DATA) | 0x80);
    out_byte(VGA_CRTC_ADDR, 0x11);
    out_byte(VGA_CRTC_DATA, in_byte(VGA_CRTC_DATA) & ~0x80);

    // make sure they remain unlocked
    regs[0x03] |= 0x80;
    regs[0x11] &= ~0x80;

    // write CRTC regs (25)
    for (int i = 0; i < 25; ++i) {
        out_byte(VGA_CRTC_ADDR, i);
        out_byte(VGA_CRTC_DATA, *regs);
        ++regs;
    }

    // write graphics controller registers (9)
    for (int i = 0; i < 9; ++i) {
        out_byte(VGA_GC_ADDR, i);
        out_byte(VGA_GC_DATA, *regs);
        ++regs;
    }

    // write attribute controller registers (21)
    for (int i = 0; i < 21; ++i) {
        in_byte(VGA_INSTAT_R);
        out_byte(VGA_AC_ADDR, i);
        out_byte(VGA_AC_DATA_W, *regs);
        ++regs;
    }

    // lock 16-color palette and unblank display
    in_byte(VGA_INSTAT_R);
    out_byte(VGA_AC_ADDR, 0x20);
}

// enable or disable cursor
void vga_set_cursor_enable(bool enable) {
    out_byte(VGA_CRTC_ADDR, 0x0a);  // cursor start
    uint8_t start = in_byte(VGA_CRTC_DATA);
    if (enable) {
        start &= ~(1U << 5);
    } else {
        start |= 1U << 5;
    }
    out_byte(VGA_CRTC_DATA, start);
}

// set the cursor position by writing to CRT Controller registers
void vga_set_cursor_pos(uint16_t pos) {
    out_byte(VGA_CRTC_ADDR, 0x0e);  // cursor location high
    out_byte(VGA_CRTC_DATA, (pos >> 8) & 0xff);
    out_byte(VGA_CRTC_ADDR, 0x0f);  // cursor location low
    out_byte(VGA_CRTC_DATA, pos & 0xff);
}

// set the memory address of the upper left character
void vga_set_start_addr(uint16_t addr) {
    out_byte(VGA_CRTC_ADDR, 0x0c);  // start address high
    out_byte(VGA_CRTC_DATA, (addr >> 8) & 0xff);
    out_byte(VGA_CRTC_ADDR, 0x0d);  // start address low
    out_byte(VGA_CRTC_DATA, addr & 0xff);
}
