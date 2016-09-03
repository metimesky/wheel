#include <drivers/console.h>
#include <drivers/vga.h>
#include <lib/string.h>

// 可以将console.c放在debug模块中，这只是启动过程中的临时终端

static char console_attr;

static int console_width;
static int console_height;

static char *frame_buf;
static size_t frame_buf_len;

static int x, y;

void console_init() {
    vga_write_regs(regs_80x25_text);
    console_width = 80;
    console_height = 25;
    frame_buf = 0xa0000 + KERNEL_VMA;
    frame_buf_len = 80*2*52;
    x = 0;
    y = 0;
    console_attr = 0x1f;    // white on blue

    vga_set_start_addr(0);
    vga_set_cursor_pos(0);

    // 清屏
    for (int i = 0; i < console_width*console_height; ++i) {
        frame_buf[2*i] = ' ';
        frame_buf[2*i+1] = console_attr;
    }
}

void console_set_attr(char attr) {
    console_attr = attr;
}

void console_scroll(int n) {
    for (int i = n; i < console_height; ++i) {
        // use memcpy later
        for (int j = 0; j < console_width; ++j) {
            //frame_buf[2*(console_width*(i-n) + j)] = frame_buf[2*(console_width*i + j)];
            //frame_buf[2*(console_width*(i-n) + j)+1] = frame_buf[2*(console_width*i + j)+1];
            memcpy(frame_buf+2*(console_width*(i-n)+j), frame_buf+2*(console_width*(i)+j), console_width*2);
        }
    }
    
    // clear last n lines
    for (int i = console_height-n; i < console_height; ++i) {
        for (int j = 0; j < console_width; ++j) {
            frame_buf[2*(console_width*i + j)] = ' ';
            frame_buf[2*(console_width*i + j)+1] = console_attr;
        }
    }
}

void console_putchar(char s) {
    switch (s) {
    case '\t':
        x += 8 - x % 8;
        break;
    case '\n':
        ++y;
        x = 0;
        break;
    default:
        frame_buf[2*(x+y*console_width)] = s;
        frame_buf[2*(x+y*console_width)+1] = console_attr;
        ++x;
        break;
    }
    if (x >= console_width) {
        y += x / console_width;
        x = x % console_width;
    }
    if (y >= console_height) {
        console_scroll(1);
        y -= 1;
    }
    vga_set_cursor_pos(x+y*console_width);
}

static char *u64_to_str(unsigned long long value, char *str, int base) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  
    /* Check whether base is valid. */
    if ((base < 2) || (base > 36)) {
        str[0] = '\0';
        return NULL;
    }

    /* Convert to string. Digits are in reverse order.  */
    int i = 0;
    unsigned int remainder = 0;
    do {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    } while (value != 0);
    str[i] = '\0';
  
    /* Reverse string.  */
    char tmp;
    int j;
    for (j=0, --i; j<i; ++j, --i) {
        tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
    }
  
    return str;
}

static char *i64_to_str(long long value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u64_to_str(-value, &buf[1], base);
    } else {
        return u64_to_str(value, buf, base);
    }
}

void console_print(const char *fmt, ...) {
    //static char buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    char *s;
    char buf[70];
    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            //buf[i] = *fmt;
            console_putchar(*fmt);
        } else {
            ++fmt;
            switch (*fmt) {
            case '\0':
                // unlucky
                break;
            case 'c':
                //buf[i] = (char) va_arg(args, int);
                console_putchar((char) va_arg(args, int));
                break;
            case '%':
                //buf[i] = '%';
                console_putchar('%');
                break;
            case 's':
                s = va_arg(args, char*);
                for (; *s; ++s) {
                    console_putchar(*s);
                }
                break;
            case 'p':
            case 'x':
            case 'X':
                u64_to_str((uint64_t) va_arg(args, void *), buf, 16);
                for (int i = 0; buf[i]; ++i) {
                    console_putchar(buf[i]);
                }
                break;
            case 'd':
            case 'i':
                i64_to_str((int64_t) va_arg(args, void *), buf, 10);
                for (int i = 0; buf[i]; ++i) {
                    console_putchar(buf[i]);
                }
                break;
            case 'u':
                u64_to_str((uint64_t) va_arg(args, void *), buf, 10);
                for (int i = 0; buf[i]; ++i) {
                    console_putchar(buf[i]);
                }
                break;
            case 'o':
                u64_to_str((uint64_t) va_arg(args, void *), buf, 8);
                for (int i = 0; buf[i]; ++i) {
                    console_putchar(buf[i]);
                }
                break;
            default:
                // unlucky
                break;
            }
        }
    }
    va_end(args);
}
