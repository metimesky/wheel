#include "string.h"
#include <stdhdr.h>

// TODO: very naive implementation, must be refactored later.

void memcpy(void *dst, void *src, int n) {
    for (int i = 0; i < n; ++i) {
        ((char *) dst)[i] = ((char *) src)[i];
    }
}

void memmove(void *dst, void *src, int n) {
    if ((int) (dst - src) < n) {
        // copy backward
        for (int i = n - 1; i >= 0; --i) {
            ((char *) dst)[i] = ((char *) src)[i];
        }
    } else {
        for (int i = 0; i < n; ++i) {
            ((char *) dst)[i] = ((char *) src)[i];
        }
    }
}

void memset(void *dst, char c, int n) {
    for (int i = 0; i < n; ++i) {
        ((char *) dst)[i] = c;
    }
}

int memcmp(const void *s1, const void *s2, int n) {
    unsigned char *u1 = (const unsigned char *) s1;
    unsigned char *u2 = (const unsigned char *) s2;
    for (; n--; u1++, u2++) {
        if (*u1 != *u2) {
            return (*u1 - *u2);
        }
    }
    return 0;
}

// copy string from src to dst at most n bytes (excluding trailing zero)
// and return the actual number of bytes copied (excluding trailing zero)
// the return value should be same as strlen(dst)
int strncpy(char *dst, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i]; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return i;
}

// calculate the length of the string, excluding trailing zero.
int strlen(const char *str) {
    int len = 0;
    while (str[len]) { ++len; }
    return len;
}

char *u32_to_str(unsigned int value, char *str, int base) {
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

char *u64_to_str(unsigned long long value, char *str, int base) {
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

char *i32_to_str(int value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u32_to_str(-value, &buf[1], base);
    } else {
        return u32_to_str(value, buf, base);
    }
}

char *i64_to_str(long long value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u64_to_str(-value, &buf[1], base);
    } else {
        return u64_to_str(value, buf, base);
    }
}

/*
int vsprintf(char *buf, const char *fmt, va_list args) {
    const char *s;
    char *str;
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        ++fmt;
        switch (*fmt) {
        case 'c':
            *str++ = (unsigned char)va_arg(args, int);
            break;
        case 's':
            s = va_arg(args, char *);
            for (int i = 0; s[i]; ++i) {
                *str++ = s[i];
            }
            break;
        case 'p':
            u64_to_str((uint64_t) va_arg(args, void *), str, 16);
            str += strlen(str);
            break;
        case '%':
            *str++ = '%';
            break;
        case 'o':
            u64_to_str(va_arg(args, uint64_t), str, 8);
            str += strlen(str);
            break;
        case 'x':
        case 'X':
            u64_to_str(va_arg(args, uint64_t), str, 16);
            str += strlen(str);
            break;
        case 'd':
        case 'i':
            i64_to_str(va_arg(args, int64_t), str, 10);
            str += strlen(str);
            break;
        case 'u':
            u64_to_str(va_arg(args, uint64_t), str, 10);
            str += strlen(str);
            break;
        default:
            *str++ = '%';
            if (*fmt) {
                *str++ = *fmt;
            } else {
                --fmt;
            }
            break;
        }
    }
    *str = '\0';
    return str - buf;
}
*/

// print to buf, at most n bytes (excluding trailing zero)
// return actual number of bytes written (excluding trailing zero)
int vsnprintf(char *buf, int n, const char *fmt, va_list args) {
    static char num[33];
    int i = 0;
    for (i = 0; i < n; ++i, ++fmt) {
        if (*fmt != '%') {
            buf[i] = *fmt;
        } else {
            ++fmt;
            switch (*fmt) {
            case 'c':
                buf[i] = (unsigned char) va_arg(args, int);
                break;
            case '%':
                buf[i] = '%';
                break;
            case 's':
                i += strncpy(&buf[i], va_arg(args, char*), n - i) - 1;
                break;
            case 'p':
            case 'x':
            case 'X':
                u64_to_str((uint64_t) va_arg(args, void *), num, 16);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'd':
            case 'i':
                i64_to_str((uint64_t) va_arg(args, void *), num, 10);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'u':
                u64_to_str((uint64_t) va_arg(args, void *), num, 10);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'o':
                u64_to_str((uint64_t) va_arg(args, void *), num, 8);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            default:
                // *str++ = '%';
                // if (*fmt) {
                //     *str++ = *fmt;
                // } else {
                //     --fmt;
                // }
                break;
            }
        }
    }
    buf[i] = '\0';
    return i;
}

int snprintf(char *buf, int n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, n, fmt, args);
    va_end(args);
}