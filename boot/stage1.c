#include <types.h>
#include "multiboot.h"

void raw_write(const char *str, char attr, int pos) {
    static char* const video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2*(pos+i)] = str[i];
        video[2*(pos+i)+1] = attr;
    }
}

char* utoa(unsigned int value, char *str, int base) {
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

char* itoa(int value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return utoa(-value, &buf[1], base);
    } else {
        return utoa(value, buf, base);
    }
}

void wheel_stage1(uint32_t eax, uint32_t ebx) {
    if (0x2badb002 != eax) {
        raw_write("Bootloader not multiboot compliant!", 0x0c, 80);
    } else {
        raw_write("Hello, world!", 0x1f, 80);
    }

    multiboot_info_t *mbi = (multiboot_info_t*)ebx;
    raw_write((char*)mbi->cmdline, 0x0b, 160);

    char buf[32];
    raw_write("lower memory:", 0x0f, 240);
    raw_write(utoa(mbi->mem_lower, buf, 16), 0x0a, 255);
    raw_write(utoa(mbi->mem_lower, buf, 10), 0x0a, 265);
    raw_write("upper memory:", 0x0f, 320);
    raw_write(utoa(mbi->mem_upper, buf, 16), 0x0a, 335);
    raw_write(utoa(mbi->mem_upper, buf, 10), 0x0a, 345);

    raw_write("mmap length:", 0x0f, 400);
    raw_write(utoa(mbi->mmap_length, buf, 10), 0x0a, 413);
    raw_write("mmap addr:", 0x0f, 440);
    raw_write(utoa(mbi->mmap_addr, buf, 16), 0x0a, 451);
}
