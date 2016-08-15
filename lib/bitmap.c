#include <lib/bitmap.h>

void bitmap_set(uint64_t *map, int start, int len) {
    // calculate the first long affected.
    uint64_t *p = map + start / BITS_PER_UINT64;
    const int end = start + len;
    int bits_to_set = BITS_PER_UINT64 - (start % BITS_PER_UINT64);
    uint64_t mask_to_set = BITMAP_FIRST_UINT64_MASK(start);

    if (len >= bits_to_set) {
        *p |= mask_to_set;
        len -= bits_to_set;
        bits_to_set = BITS_PER_UINT64;
        mask_to_set = ~0UL;
        ++p;
    }
    for (; len >= bits_to_set; ++p, len -= bits_to_set) {
        *p |= mask_to_set;
    }
    if (len) {
        mask_to_set &= BITMAP_LAST_UINT64_MASK(end);
        *p |= mask_to_set;
    }
}

void bitmap_clear(uint64_t *map, int start, int len) {
    // calculate the first long affected.
    uint64_t *p = map + start / BITS_PER_UINT64;
    const int end = start + len;
    int bits_to_clear = BITS_PER_UINT64 - (start % BITS_PER_UINT64);
    uint64_t mask_to_clear = ~BITMAP_FIRST_UINT64_MASK(start);

    if (len >= bits_to_clear) {
        *p &= mask_to_clear;
        len -= bits_to_clear;
        bits_to_clear = BITS_PER_UINT64;
        mask_to_clear = 0UL;
        ++p;
    }
    for (; len >= bits_to_clear; ++p, len -= bits_to_clear) {
        *p &= mask_to_clear;
    }
    if (len) {
        mask_to_clear |= ~BITMAP_LAST_UINT64_MASK(end);
        *p &= mask_to_clear;
    }
}
