#include <lib/string.h>

typedef uint64_t WORD;
#define WORD_SIZE sizeof(uint64_t)
#define WORD_MASK (WORD_SIZE - 1)

void *memcpy(void *dst, const void *src, size_t count) {
    char *d = (char *)dst;
    const char *s = (const char *)src;
    int len;

    if (count == 0 || dst == src) { return dst; }

    // 如果src或dst的地址没有对齐
    if (((WORD)d | (WORD)s) & WORD_MASK) {
        // 判断能否去掉前几个字节让src和dst对齐
        if ((((WORD)d ^ (WORD)s) & WORD_MASK) || (count < WORD_SIZE)) {
            // 不能，全部按字节复制
            len = count;
        } else {
            // 先复制len个字节，对齐到WORD
            len = WORD_SIZE - ((WORD)d & WORD_MASK);
        }
        count -= len;
        for (; len > 0; --len) { *d++ = *s++; }
    }

    for (len = count / WORD_SIZE; len > 0; --len) {
        *(WORD *)d = *(WORD *)s;
        d += WORD_SIZE;
        s += WORD_SIZE;
    }

    for (len = count & WORD_MASK; len > 0; --len) { *d++ = *s++; }

    return dst;
}