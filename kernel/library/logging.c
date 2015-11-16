#include "logging.h"
#include <common/stdhdr.h>
#include <driver/console/console.h>

// TODO: we need to be able to output to other drivers

/**
    This function has to consider buffer.
 */
#define BUF_SIZE 1024

void log(const char *fmt, ...) {
    static char buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int i = 0;
    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            buf[i] = *fmt;
        } else {
            ++fmt;
            switch (*fmt) {
            case '\0':
                // unlucky
                break;
            case 'c':
                buf[i] = va_arg(args, char);
                break;
            case '%':
                buf[i] = '%';
                break;
            case 's':
                // flush current buffer
                buf[i] = '\0';
                console_put_string(buf);
                // output the string argument
                console_put_string(va_arg(args, char*));
                // clear buffer
                i = -1;
                break;
            case 'p':
            case 'x':
            case 'X':
                // flush current buffer
                buf[i] = '\0';
                console_put_string(buf);
                // convert the argument to string
                u64_to_str((uint64_t) va_arg(args, void *), buf, 16);
                // output number to console
                console_put_string(buf);
                // clear buffer
                i = -1;
                break;
            case 'd':
            case 'i':
                // flush current buffer
                buf[i] = '\0';
                console_put_string(buf);
                // convert the argument to string
                i64_to_str((uint64_t) va_arg(args, void *), buf, 10);
                // output number to console
                console_put_string(buf);
                // clear buffer
                i = -1;
                break;
            case 'u':
                // flush current buffer
                buf[i] = '\0';
                console_put_string(buf);
                // convert the argument to string
                u64_to_str((uint64_t) va_arg(args, void *), buf, 10);
                // output number to console
                console_put_string(buf);
                // clear buffer
                i = -1;
                break;
            case 'o':
                // flush current buffer
                buf[i] = '\0';
                console_put_string(buf);
                // convert the argument to string
                u64_to_str((uint64_t) va_arg(args, void *), buf, 8);
                // output number to console
                console_put_string(buf);
                // clear buffer
                i = -1;
                break;
            default:
                // unlucky
                break;
            }
        }
        ++i;
        if (i == BUF_SIZE - 1) {
            // if the current buffer is too large, then flush it
            buf[i] = '\0';
            console_put_string(buf);
            // clear buffer;
            i = 0;
        }
    }
    buf[i] = '\0';
    console_put_string(buf);
    console_put_char('\n');
    va_end(args);
}
