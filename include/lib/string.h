#ifndef __STRING_H__
#define __STRING_H__

#include <wheel.h>

extern void *memcpy(void *dst, const void *src, size_t count);
extern void *memset(void *dst, int c, size_t count);
extern int memcmp(const void *ptr1, const void *ptr2, size_t count);

extern size_t strlen(const char *str);
extern size_t strnlen(const char *str, size_t count);
extern int strcmp(const char *str1, const char *str2);
extern int strncmp(const char *str1, const char *str2, size_t count);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *dst, const char *src, size_t count);
extern char *strcat(char *dst, const char *src);

extern long strtol(const char *str, char **end, int base);
extern unsigned long strtoul(const char *str, char **end, int base);
extern long long strtoll(const char *str, char **end, int base);
extern unsigned long long strtoull(const char *str, char **end, int base);

#endif