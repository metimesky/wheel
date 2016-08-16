#include <lib/string.h>

int strncmp(const char *str1, const char *str2, size_t count) {
    for (int i = 0; str1[i] && str2[i] && i < count; ++i) {
        if (str1[i] > str2[i]) {
            return 1;
        } else if (str1[i] < str2[i]) {
            return -1;
        }
    }
    return 0;
}