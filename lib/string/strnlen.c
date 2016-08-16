#include <lib/string.h>

size_t strnlen(const char *str, size_t count) {
    size_t len;
    for (len = 0; str[len] && len <= count; ++len) { }
    return len;
}