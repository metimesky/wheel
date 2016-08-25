#include <lib/string.h>

int strcmp(const char *str1, const char *str2) {
    size_t n1 = strlen(str1);
    size_t n2 = strlen(str2);
    
    int res = memcmp(str1, str2, (n1 < n2) ? n1 : n2);
    if (res != 0){
        return res;
    }
    if (n1 > n2){
        return 1;
    } else if (n1 < n2) {
        return -1;
    }
    return 0;
}