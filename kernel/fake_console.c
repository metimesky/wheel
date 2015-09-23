#include <type.h>
#include "fake_console.h"

char * const video = (char *) 0xb8000;
int cursor = 0;

void print(const char *str) {
    for (int i = 0; str[i]; ++i, ++cursor) {
        video[2*cursor] = str[i];
        video[2*cursor+1] = 0x0f;
    }
}

void println(const char *str) {
    for (int i = 0; str[i]; ++i, ++cursor) {
        video[2*cursor] = str[i];
        video[2*cursor+1] = 0x0f;
    }
    cursor += 80 - cursor % 80;
}

