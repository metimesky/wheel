#include <lib/string.h>

int memcmp(const void *ptr1, const void *ptr2, size_t count) {
    const unsigned char *vptr1 = (const unsigned char *) ptr1;
    const unsigned char *vptr2 = (const unsigned char *) ptr2;
    for (; count; --count, ++vptr1, ++vptr2) {
        if (*vptr1 > *vptr2) {
            return 1;
        } else if (*vptr1 < *vptr2) {
            return -1;
        }
    }
    return 0;
}