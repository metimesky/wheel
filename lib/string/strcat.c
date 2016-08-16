#include <lib/string.h>

char *strcat(char *dst, const char *src) {
    char *tmp = dst;

    for (; *dst; ++dst) { }
    while (*dst++ = *src++) { }

    return tmp;
}