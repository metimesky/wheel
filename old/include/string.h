#ifndef __STRING_H__
#define __STRING_H__ 1

#include <env.h>

size_t strlen(const char* s);

char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int val, size_t n);

char* u32_to_str(unsigned int value, char *str, int base);
char* u64_to_str(unsigned long long value, char *str, int base);
char* i32_to_str(int value, char *str, int base);
char* i64_to_str(long long value, char *str, int base);

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif // __STRING_H__