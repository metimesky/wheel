#include <stdhdr.h>
#include "fake_console.h"
#include <util.h>

char * const video = (char *) 0xb8000;
int cursor = 0;

void print(const char *str) {
    for (int i = 0; str[i]; ++i, ++cursor) {
        switch (str[i]) {
        case '\n':
            cursor += 79 - cursor % 80;
            break;
        case '\t':
            cursor += 7 - cursor % 8;
            break;
        default:
            video[2*cursor] = str[i];
            video[2*cursor+1] = 0x0f;
            break;
        }
    }
}

void println(const char *str) {
    for (int i = 0; str[i]; ++i, ++cursor) {
        video[2*cursor] = str[i];
        video[2*cursor+1] = 0x0f;
    }
    cursor += 80 - cursor % 80;
}

void printf(const char *fmt, ...) {
    va_list args;
    static char buf[1024];
    va_start(args, fmt);
    vsnprintf(buf, 1023, fmt, args);
    va_end(args);
    print(buf);
}