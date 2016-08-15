#ifndef __VGA_H__
#define __VGA_H__

#include <wheel.h>

extern uint8_t regs_80x25_text[];
extern uint8_t regs_90x30_text[];

extern void vga_read_regs(uint8_t *regs);
extern void vga_write_regs(uint8_t *regs);
extern void vga_set_cursor_enable(bool enable);
extern void vga_set_cursor_pos(uint16_t pos);
extern void vga_set_start_addr(uint16_t addr);

#endif
