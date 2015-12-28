#ifndef __STRING_H__
#define __STRING_H__ 1

#include <common/stdhdr.h>

void memcpy(void *dst, void *src, int n);
void memmove(void *dst, void *src, int n);
void memset(void *dst, char c, int n);
int memcmp(const void *s1, const void *s2, int n);

// int strncpy(char *dst, const char *src, int n);
// int strlen(const char *str);

int
tolower (
    int                     c);

int
toupper (
    int                     c);

char *
strcat (
    char                    *DstString,
    const char              *SrcString);

char *
strchr (
    const char              *String,
    int                     ch);

char *
strcpy (
    char                    *DstString,
    const char              *SrcString);

int
strcmp (
    const char              *String1,
    const char              *String2);

int
strlen (
    const char              *String);

char *
strncat (
    char                    *DstString,
    const char              *SrcString,
    int               Count);

int
strncmp (
    const char              *String1,
    const char              *String2,
    int               Count);

char *
strncpy (
    char                    *DstString,
    const char              *SrcString,
    int               Count);

char *
strstr (
    char                    *String1,
    char                    *String2);

char *u32_to_str(unsigned int value, char *str, int base);
char *u64_to_str(unsigned long long value, char *str, int base);
char *i32_to_str(int value, char *buf, int base);
char *i64_to_str(long long value, char *buf, int base);

int vsnprintf(char *buf, int n, const char *fmt, va_list args);

#endif // __STRING_H__