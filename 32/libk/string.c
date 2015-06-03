#include <types.h>

#define UNALIGNED(x) ((long)x & (sizeof(long)-1))

char* uint_to_str(unsigned int value, char *str, int base) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  
    /* Check base is supported. */
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

char* int_to_str(int value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        uint_to_str(-value, &buf[1], base);
    } else {
        uint_to_str(value, buf, base);
    }
}
