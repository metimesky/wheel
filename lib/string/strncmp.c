#include <lib/string.h>

int strncmp(const char *str1, const char *str2, size_t count) {
    for (; count > 0; ++str1, ++str2, --count) {
        if (*str1 != *str2) {
            return ((*(unsigned char *)str1 < *(unsigned char *)str2) ? -1 : +1);
        } else if (*str1 == '\0') {
            return 0;
        }
    }
    return 0;
}