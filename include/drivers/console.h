#ifndef __CONSOLE_H__
#define __CONSOLE_H__

extern void console_init();
extern void console_set_attr(char attr);
extern void console_scroll(int n);
extern void console_putchar(char s);
extern void console_print(const char *fmt, ...);

#endif
