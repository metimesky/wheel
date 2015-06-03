#include <types.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(x) ((long)x&(sizeof(long)-1))

void* memcpy(void *dst, void *src, size_t n) {
    void *dst_bak = dst;

    if (n >= sizeof(long)*4 && !UNALIGNED(src) && !UNALIGNED(dst)) {
        while (n >= sizeof(long)*4) {
            *(long*)dst++ = *(long*)src++;
            *(long*)dst++ = *(long*)src++;
            *(long*)dst++ = *(long*)src++;
            *(long*)dst++ = *(long*)src++;
            n -= sizeof(long)*4;
        }
        while (n >= sizeof(long)) {
            *(long*)dst++ = *(long*)src++;
            n -= sizeof(long);
        }
    }

    while (n--) {
        *(char*)dst++ = *(char*)src++;
    }

    return dst_bak;
}

void* memset(void *buf, unsigned char byte, size_t n) {
    void *buf_bak = buf;

    while (UNALIGNED(buf)) {
        if (n--) {
            *(char*)buf++ = (char)byte;
        } else {
            return buf;
        }
    }

    if (n>=sizeof(long)) {
        unsigned long dword = byte;
        dword |= dword << 8;
        dword |= dword << 16;
        
        while (n >= sizeof(long)*4) {
            *(long*)buf++ = dword;
            *(long*)buf++ = dword;
            *(long*)buf++ = dword;
            *(long*)buf++ = dword;
            n -= 4*sizeof(long);
        }

        while (n >= sizeof(long)) {
            *(long*)buf++ = dword;
            n -= sizeof(long);
        }
    }

    while (n--) {
        *(char*)buf++ = byte;
    }
}
