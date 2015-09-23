#include "string.h"
#include <type.h>

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

int memcmp(void *x, void *y, int n) {
    static int i, delta;
    for (i = 0, delta = 0; delta == 0 && i < n; ++i) {
        delta = ((char *) x)[i] - ((char *) y)[i];
    }
    return delta;
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