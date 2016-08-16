#include <lib/string.h>

char *strncpy(char *dst, const char *src, size_t count) {
    char *tmp = dst;
    while (count-- && (*dst++ = *src++)) { }
    return tmp;
}