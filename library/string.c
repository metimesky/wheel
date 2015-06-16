#include <env.h>
#include <string.h>

// TODO: these functions need optimization!

// return true if the long contains NULL character
#define HASNULL(x) (((x) - 0x0101010101010101) & ~(x) & 0x8080808080808080)

size_t strlen(const char* s) {
    const char *start = s;
    while ((size_t) s & (sizeof(long) - 1)) {
        if (!*s) {
            return s - start;
        }
        ++s;
    }
    unsigned long *aligned_s = (unsigned long *) s;
    for (; !HASNULL(*aligned_s); ++aligned_s) {}
    s = (const char *) aligned_s;
    for (; *s; ++s) {}
    return s - start;
}

char *strcpy(char* dst, const char *src) {
    int i;
    for (i = 0; src[i]; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

// terminating null is only added when src is shorter than n
char *strncpy(char *dst, const char *src, size_t n) {
    size_t i;
    for (i = 0; src[i] && i < n; ++i) {
        dst[i] = src[i];
    }
    if (i < n) {
        dst[i] = '\0';
    }
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    char *t = (char *) dst;
    char *s = (char *) src;
    if (!(n < sizeof(long) || (size_t) t & (sizeof(long) - 1) || (size_t) s & (sizeof(long) - 1))) {
        unsigned long *aligned_t = (unsigned long *) t;
        const unsigned long *aligned_s = (const unsigned long *) s;
        for (; n >= sizeof(long) * 4; n -= sizeof(long) * 4) {
            *aligned_t++ = *aligned_s++;
            *aligned_t++ = *aligned_s++;
            *aligned_t++ = *aligned_s++;
            *aligned_t++ = *aligned_s++;
        }
        for (; n >= sizeof(long); n -= sizeof(long)) {
            *aligned_t++ = *aligned_s++;
        }
        t = (char *) aligned_t;
        s = (char *) aligned_s;
    }
    for (; n > 0; --n) {
        *(char *) t++ = *(char *) s++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    char *ptr = (char *) dst;
    for (; ((size_t) ptr & (sizeof(long) - 1)) && (n > 0); --n) {
        *ptr++ = (char)val;
    }
    if (n >= sizeof(long)) {
        unsigned long *aligned_ptr = (unsigned long *) ptr;
        unsigned long lval = val & 0xff;
        for (int i = 8; i < sizeof(long) * 8; i <<= 1) {
            lval |= lval << i;
        }
        for (; n >= sizeof(long) * 4; n -= sizeof(long) * 4) {
            *aligned_ptr++ = lval;
            *aligned_ptr++ = lval;
            *aligned_ptr++ = lval;
            *aligned_ptr++ = lval;
        }
        for (; n >= sizeof(long); n -= sizeof(long)) {
            *aligned_ptr++ = lval;
        }
        ptr = (char *) aligned_ptr;
    }
    for (; n > 0; --n) {
        *ptr++ = (char) val;
    }
    return dst;
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


// the syntax for a placeholder is:
// %[flags][width][.precision][length]type
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

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(buf, fmt, args);
    va_end(args);
    return ret;
}
