#ifndef __BITMAP_H__
#define __BITMAP_H__ 1

#include <env.h>

// how many bits are there in a long
#define BITS_PER_UINT64 (8 * sizeof(long))

// calculate how many uint64 contain n bits, rounded upward
#define BITS_TO_UINT64(n) (((n) + BITS_PER_UINT64 - 1) / BITS_PER_UINT64)

// bitmap operations often start or end in middle of a long, while
// actions are done in long's manner, so we have to clear extra bits
// in first and last long.
#define BITMAP_FIRST_UINT64_MASK(i) (~0UL << ((i) % BITS_PER_UINT64))
#define BITMAP_LAST_UINT64_MASK(i) \
    (((i) % BITS_PER_UINT64) ? (1UL << ((i) % BITS_PER_UINT64)) - 1 : ~0UL)

// operation on individual bit
#define BIT_TEST(map, i)    ((map)[(i)/BITS_PER_UINT64] & (1UL << ((i) % BITS_PER_UINT64)))
#define BIT_SET(map, i)     (map)[(i)/BITS_PER_UINT64] |= 1UL << ((i) % BITS_PER_UINT64)
#define BIT_CLEAR(map, i)   (map)[(i)/BITS_PER_UINT64] &= ~(1UL << ((i) % BITS_PER_UINT64))

// set entire bitmap to zeros, including trailing extra bits
static inline void bitmap_zero(uint64_t *dst, int nbits) {
    if (nbits <= BITS_PER_UINT64) {
        *dst = 0UL;
    } else {
        int len = BITS_TO_UINT64(nbits) * sizeof(uint64_t);
        __builtin_memset(dst, 0, len);
    }
}

// set entire bitmap to ones, excluding trailing extra bits
static inline void bitmap_fill(uint64_t *dst, int nbits) {
    size_t nlongs = BITS_TO_UINT64(nbits);
    if (nbits < BITS_PER_UINT64) {
        int len = (nlongs - 1) * sizeof(uint64_t);
        __builtin_memset(dst, 0xff,  len);
    }
    dst[nlongs - 1] = BITMAP_LAST_UINT64_MASK(nbits);
}

// copy bitmap from src to dst, including trialing zeros
static inline void bitmap_copy(uint64_t *dst, const uint64_t *src, int nbits) {
    if (nbits < BITS_PER_UINT64) {
        *dst = *src;
    } else {
        int len = BITS_TO_UINT64(nbits) * sizeof(uint64_t);
        __builtin_memcpy(dst, src, len);
    }
}

void bitmap_set(uint64_t *map, int start, int len);
void bitmap_clear(uint64_t *map, int start, int len);

#endif // __BITMAP_H__
