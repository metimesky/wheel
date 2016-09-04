#ifndef __BITS_H__
#define __BITS_H__

#define clz(x) __builtin_clz(x)
#define ctz(x) __builtin_ctz(x)

#define BIT(x, bit) ((x) & (1UL << (bit)))
#define BIT_SHIFT(x, bit) (((x) >> (bit)) & 1)
#define BITS(x, high, low) ((x) & (((1UL<<((high)+1))-1) & ~((1UL<<(low))-1)))
#define BITS_SHIFT(x, high, low) (((x) >> (low)) & ((1UL<<((high)-(low)+1))-1))
#define BIT_SET(x, bit) (((x) & (1UL << (bit))) ? 1 : 0)

#define BITMAP_BITS_PER_WORD (sizeof(unsigned long) * 8)
#define BITMAP_NUM_WORDS(x) (((x) + BITMAP_BITS_PER_WORD - 1) / BITMAP_BITS_PER_WORD)
#define BITMAP_WORD(x) ((x) / BITMAP_BITS_PER_WORD)
#define BITMAP_BIT_IN_WORD(x) ((x) & (BITMAP_BITS_PER_WORD - 1))

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITMAP_BITS_PER_WORD))
#define BITMAP_LAST_WORD_MASK(nbits) (((nbits) % BITMAP_BITS_PER_WORD) ? (1UL<<((nbits) % BITMAP_BITS_PER_WORD))-1 : ~0UL)

#define BITMAP_BITS_PER_INT (sizeof(unsigned int) * 8)
#define BITMAP_BIT_IN_INT(x) ((x) & (BITMAP_BITS_PER_INT - 1))
#define BITMAP_INT(x) ((x) / BITMAP_BITS_PER_INT)

#define BIT_MASK(x) (((x) >= sizeof(unsigned long) * 8) ? (0UL-1) : ((1UL << (x)) - 1))

static inline void bitmap_set(unsigned long *bitmap, int start, int count) {
    unsigned long *p = bitmap + BITMAP_WORD(start);
    const long size = start + count;
    int bits_to_set = BITMAP_BITS_PER_WORD - (start % BITMAP_BITS_PER_WORD);
    unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

    while (count - bits_to_set >= 0) {
        *p |= mask_to_set;
        count -= bits_to_set;
        bits_to_set = BITMAP_BITS_PER_WORD;
        mask_to_set = ~0UL;
        ++p;
    }
    if (count) {
        mask_to_set &= BITMAP_LAST_WORD_MASK(size);
        *p |= mask_to_set;
    }
}

static inline void bitmap_clear(unsigned long *bitmap, int start, int count) {
    unsigned long *p = bitmap + BITMAP_WORD(start);
    const long size = start + count;
    int bits_to_clear = BITMAP_BITS_PER_WORD - (start % BITMAP_BITS_PER_WORD);
    unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

    while (count - bits_to_clear >= 0) {
        *p &= ~mask_to_clear;
        count -= bits_to_clear;
        bits_to_clear = BITMAP_BITS_PER_WORD;
        mask_to_clear = ~0UL;
        ++p;
    }
    if (count) {
        mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
        *p &= ~mask_to_clear;
    }
}

static inline int bitmap_test(unsigned long *bitmap, int bit) {
    return BIT_SET(bitmap[BITMAP_WORD(bit)], BITMAP_BIT_IN_WORD(bit));
}

// 返回二进制表示中第一个0的下标，如果x全一，则返回-1
static inline unsigned long __find_first_zero(unsigned long x) {
    return __builtin_ffsl(~x) - 1;
}

static inline int bitmap_find_first_zero(unsigned long *bitmap, int count) {
    for (int i = 0; i < BITMAP_NUM_WORDS(count); ++i) {
        if (bitmap[i] == ~0UL) {
            continue;
        }
        int bit = i * BITMAP_BITS_PER_WORD + __find_first_zero(bitmap[i]);
        if (bit < count) {
            return bit;
        }
    }
    return -1;
}

// 返回二进制表示中第一个1的下标，如果x全零，则返回-1
static inline unsigned long __find_first_set(unsigned long x) {
    return __builtin_ffsl(x) - 1;
}

static inline int bitmap_find_first_set(unsigned long *bitmap, int count) {
    for (int i = 0; i < BITMAP_NUM_WORDS(count); ++i) {
        if (bitmap[i] == 0UL) {
            continue;
        }
        int bit = i * BITMAP_BITS_PER_WORD + __find_first_set(bitmap[i]);
        if (bit < count) {
            return bit;
        }
    }
    return -1;
}

#endif