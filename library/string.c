#include <env.h>
#include <string.h>

int strlen(const char* s) {
    int i;
    for (i = 0; s[i]; ++i) {}
    return i;
}

char* strcpy(char* dst, const char* src) {
    int i;
    for (i = 0; src[i]; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

// terminating null is only added when src is shorter than n
char* strncpy(char* dst, const char* src, int n) {
    int i;
    for (i = 0; src[i] && i < n; ++i) {
        dst[i] = src[i];
    }
    if (i < n) {
        dst[i] = '\0';
    }
    return dst;
}

char* u32_to_str(unsigned int value, char *str, int base) {
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

char* u64_to_str(unsigned long long value, char *str, int base) {
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

char* i32_to_str(int value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u32_to_str(-value, &buf[1], base);
    } else {
        return u32_to_str(value, buf, base);
    }
}

char* i64_to_str(long long value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u64_to_str(-value, &buf[1], base);
    } else {
        return u64_to_str(value, buf, base);
    }
}
/*
int vsprintf(char *buf, const char *fmt, va_list args) {
    const char *p = fmt ? fmt : "";
    for (; *p; ++p) {
        if (*p != '%') {

        }
    }
}

// string buf is at most n chars long, leaving room for the trailing NULL.
// return value does not contain the trailing null.
int snprintf_private(char *buf, int n, const char *fmt, va_list args) {
    char num_buf[65];
    int idx = 0;
    int l = 0;
    int j = 0;
    for (int i = 0; fmt[i] && j < n - 1; ++i) {
        if (fmt[i] == '%') {
            ++i;
            switch (fmt[i]) {
            case '\0':
                // trailing percent mark
                buf[j] = '%';
                ++j;
                buf[j] = '\0';
                return j;
                break;
            case 'd':
            case 'i':
                // signed decimal integer
                i32_to_str(num_buf, va_arg(args, int), 10);
                // ++idx;
                goto append_num_buf;
                break;
            case 'u':
                // unsigned decimal integer
                u32_to_str(num_buf, va_arg(args, unsigned int), 10);
                // ++idx;
                goto append_num_buf;
                break;
            case 'o':
                // unsigned octal integer
                u32_to_str(num_buf, va_arg(args, unsigned int), 8);
                // ++idx;
                goto append_num_buf;
                break;
            case 'x':
                // unsigned hexademical integer
                u32_to_str(num_buf, va_arg(args, unsigned int), 16);
                // ++idx;
                goto append_num_buf;
append_num_buf:
                strncpy(&buf[j], num_buf, n - j - 1);
                l = strlen(num_buf);
                if (j + l >= n - 1) {
                    buf[n-1] = '\0';
                    return n - 1;
                } else {
                    j += l;
                }
                break;
            case 'c':
                // character
                buf[j] = va_arg(args, char);
                ++j;
                // ++idx;
                break;
            case 's':
                // string
            {
                const char *str = va_arg(args, char*);
                strncpy(&buf[j], str, n - j - 1);
                l = strlen(str);
            }
                if (j + l >= n - 1) {
                    buf[n-1] = '\0';
                    return n - 1;
                } else {
                    j += l;
                }
                break;
            case '%':
                // percent mark
                buf[j] = '%';
                ++j;
                break;
            default:
                break;
            }
        } else {
            buf[j] = fmt[i];
            ++j;
        }
    }
    buf[j] = '\0';
    return j;
}

int sprintf(char *s, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = snprintf_private(s, INT32_MAX, fmt, &args[1]);
    va_end(args);
    return ret;
}

int snprintf(char *s, int n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = snprintf_private(s, n, fmt, &args[1]);
    va_end(args);
    return ret;
}
*/