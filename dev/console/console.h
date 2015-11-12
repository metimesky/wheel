#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/**
    This file implements pretty basic console.
    It only operate in VGA 80x25 text mode, which is the initial default.
    This driver is very basic and is intended to be started first. When the
    kernel finished initializing, this driver is replaced with graphical
    environment (maybe)
 */

void console_init();
int console_get_width();
int console_get_height();
void console_set_cursor(int x, int y);

#endif // __CONSOLE_H__