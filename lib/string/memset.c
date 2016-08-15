#include <lib/string.h>

void *memset(void *dst, int c, size_t count) {
    char *d = (char *) dst;
    size_t len = (-(size_t)dst) & (sizeof(size_t) - 1);
    size_t cc = c & 0xff;

    if (count > len) {
        cc |= cc << 8;
        cc |= cc << 16;
        cc |= cc << 32;

        // 填充开头未对齐的部分
        for (; len > 0; --len) { *d++ = c; }
        count -= len;

        // 以size_t为单位填充
        for (len = count/sizeof(size_t); len > 0; --len) {
            *((size_t *)d) = cc;
            d += sizeof(size_t);
        }

        count &= sizeof(size_t) - 1;
    }

    // 按字节填充剩余部分
    for (; count > 0; --count) { *d++ = c; }

    return dst;
}
