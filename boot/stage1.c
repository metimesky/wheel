/**
 */
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

char* ultoa(unsigned long long value, char *str, int base) {
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
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        raw_write("Bootloader magic number is invalid.", 0x4e, 0);
        while (1) {}
    }

    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
    char buf[64];

    int line = 0;

    // are `mem_*` valid?
    if (mbi->flags & (1 << 0)) {
        raw_write("lower memory (KB):", 0x0f, 80*line);
        raw_write(utoa(mbi->mem_lower, buf, 10), 0x0f, 80*line + 19);
        raw_write("upper memory (KB):", 0x0f, 80*line + 30);
        raw_write(utoa(mbi->mem_upper, buf, 10), 0x0f, 80*line + 49);
        ++line;
    }

    /* Are mmap * valid? */
    if (mbi->flags & (1 << 6)) {
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
        while ((uint32_t) mmap < mbi->mmap_addr + mbi->mmap_length) {
            raw_write("Addr:", 0x0d, 80*line);
            raw_write(ultoa(mmap->addr, buf, 16), 0x0d, 80*line+6);
            raw_write("Size:", 0x0d, 80*line+30);
            raw_write(ultoa(mmap->len, buf, 16), 0x0d, 80*line+36);
            if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
                raw_write("OK", 0x0a, 80*line+78);
            } else {
                raw_write("NO", 0x0c, 80*line+78);
            }
            mmap = (multiboot_memory_map_t *)((uint32_t) mmap + mmap->size + sizeof(uint32_t));
            ++line;
        }
    }
}
