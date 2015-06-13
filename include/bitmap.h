#ifndef __BITMAP_H__
#define __BITMAP_H__ 1

#include <env.h>

/* bitmap is of type long, and is aligned.
 */

// how many bits are there in a long
#define BITS_PER_LONG (8 * sizeof(long))

// calculate how many longs contain n bits, rounded upward
#define BITS_TO_LONGS(n) (((n) + BITS_PER_LONG - 1) / BITS_PER_LONG)

// bitmap operations often start or end in middle of a long, while
// actions are done in long's manner, so we have to clear extra bits
// in first and last long.
#define BITMAP_FIRST_LONG_MASK(i) (~0UL << ((i) % BITS_PER_LONG))
#define BITMAP_LAST_LONG_MASK(i) \
    (((i) % BITS_PER_LONG) ? (1UL << ((i) % BITS_PER_LONG))-1 : ~0UL)

// set entire bitmap to zeros, including trailing extra bits
static inline void bitmap_zero(unsigned long *dst, int nbits) {
    if (nbits < BITS_PER_LONG) {
        *dst = 0UL;
    } else {
        int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
        __builtin_memset(dst, 0, len);
    }
}

// set entire bitmap to ones, excluding trailing extra bits
static inline void bitmap_fill(unsigned long *dst, int nbits) {
    size_t nlongs = BITS_TO_LONGS(nbits);
    if (nbits < BITS_PER_LONG) {
        int len = (nlongs - 1) * sizeof(unsigned long);
        __builtin_memset(dst, 0xff,  len);
    }
    dst[nlongs - 1] = BITMAP_LAST_LONG_MASK(nbits);
}

// copy bitmap from src to dst, including trialing zeros
static inline void bitmap_copy(unsigned long *dst, const unsigned long *src, int nbits) {
    if (nbits < BITS_PER_LONG) {
        *dst = *src;
    } else {
        int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
        __builtin_memcpy(dst, src, len);
    }
}

void bitmap_set(size_t *bitmap, size_t bit);
void bitmap_clear(size_t *bitmap, size_t bit);

#endif // __BITMAP_H__