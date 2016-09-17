#include <wheel.h>

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

char* _sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *s;
    int i = 0;
    //char buf[70];
    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            buf[i] = *fmt;
            ++i;
            // console_putchar(*fmt);
        } else {
            ++fmt;
            switch (*fmt) {
            case '\0':
                // unlucky
                break;
            case 'c':
                buf[i] = (char) va_arg(args, int);
                ++i;
                break;
            case '%':
                buf[i] = '%';
                ++i;
                break;
            case 's':
                s = va_arg(args, char*);
                for (; *s; ++s) {
                    buf[i] = *s;
                    ++i;
                }
                break;
            case 'p':
            case 'x':
            case 'X':
                u64_to_str((uint64_t) va_arg(args, void *), &buf[i], 16);
                while (buf[i]) { ++i; }
                break;
            case 'd':
            case 'i':
                i64_to_str((int64_t) va_arg(args, void *), &buf[i], 10);
                while (buf[i]) { ++i; }
                break;
            case 'u':
                u64_to_str((uint64_t) va_arg(args, void *), &buf[i], 10);
                while (buf[i]) { ++i; }
                break;
            case 'o':
                u64_to_str((uint64_t) va_arg(args, void *), &buf[i], 8);
                while (buf[i]) { ++i; }
                break;
            default:
                // unlucky
                break;
            }
        }
    }
    va_end(args);
    buf[i] = '\0';
    return buf;
}
