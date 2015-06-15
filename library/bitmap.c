#include "bitmap.h"

void linux_bitmap_set(unsigned long *map, int start, int nr) {
    unsigned long *p = map + (start / BITS_PER_LONG);
    const int size = start + nr;
    int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_set = BITMAP_FIRST_LONG_MASK(start);

    while (nr - bits_to_set >= 0) {
        *p |= mask_to_set;
        nr -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_set &= BITMAP_LAST_LONG_MASK(size);
        *p |= mask_to_set;
    }
}

void bitmap_set(unsigned long *map, int start, int len) {
    // calculate the first long affected.
    unsigned long *p = map + start / BITS_PER_LONG;
    const int end = start + len;
    int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_set = BITMAP_FIRST_LONG_MASK(start);

    if (len >= bits_to_set) {
        *p |= mask_to_set;
        len -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        ++p;
    }
    for (; len >= bits_to_set; ++p, len -= bits_to_set) {
        *p |= mask_to_set;
    }
    if (len) {
        mask_to_set &= BITMAP_LAST_LONG_MASK(end);
        *p |= mask_to_set;
    }
}

void bitmap_clear(unsigned long *map, int start, int len) {
    // calculate the first long affected.
    unsigned long *p = map + start / BITS_PER_LONG;
    const int end = start + len;
    int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_clear = ~BITMAP_FIRST_LONG_MASK(start);

    if (len >= bits_to_clear) {
        *p &= mask_to_clear;
        len -= bits_to_clear;
        bits_to_clear = BITS_PER_LONG;
        mask_to_clear = 0UL;
        ++p;
    }
    for (; len >= bits_to_clear; ++p, len -= bits_to_clear) {
        *p &= mask_to_clear;
    }
    if (len) {
        mask_to_clear |= ~BITMAP_LAST_LONG_MASK(end);
        *p &= mask_to_clear;
    }
}
