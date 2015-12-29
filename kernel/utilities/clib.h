#ifndef __CLIB_H__
#define __CLIB_H__ 1

#include <utilities/env.h>

// ctype functions from libosdev, translated to macros
#define isblank(c) ((c)==' ' || (c)=='\t')
#define iscntrl(c) (0x0<=(c) && (c)<=0x8)
#define ispunct(c) ((0x21<=(c) && (c)<=0x2f) || (0x3a<=(c) && (c)<=0x40) || (0x5b<=(c) && (c)<=0x60) || (0x7b<=(c) && (c)<=0x7e))
#define isspace(c) ((c)==' ' || (c)=='\t' || (c)=='\r' || (c)=='\n' || (c)=='\f' || (c)=='\v')
#define islower(c) ('a'<=(c) && (c)<='z')
#define isupper(c) ('A'<=(c) && (c)<='Z')
#define isdigit(c) ('0'<=(c) && (c)<='9')
#define isxdigit(c) (isdigit(c) || ('a'<=(c) && (c)<='f') || ('A'<=(c) && (c)<='F'))
#define isalpha(c) (islower(c) || isupper(c))
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isgraph(c) (ispunct(c) || isalnum(c))
#define isprint(c) (isgraph(c) || isspace(c))

// these two need to be implemented as function, as ACPICA calls them
// #define tolower(c) (isupper(c) ? (c)+0x20 : (c))
// #define toupper(c) (islower(c) ? (c)-0x20 : (c))
extern char tolower(char c);
extern char toupper(char c);

// data accessing helpers
#define DATA_U8(x)  (* ((uint8_t *)  (x)))
#define DATA_U16(x) (* ((uint16_t *) (x)))
#define DATA_U32(x) (* ((uint32_t *) (x)))
#define DATA_U64(x) (* ((uint64_t *) (x)))
#define DATA_I8(x)  (* ((int8_t *)  (x)))
#define DATA_I16(x) (* ((int16_t *) (x)))
#define DATA_I32(x) (* ((int32_t *) (x)))
#define DATA_I64(x) (* ((int64_t *) (x)))

// memory functions
extern void *memcpy(void *dst, const void *src, size_t n);
extern void *memmove(void *dst, const void *src, size_t n);
extern int memcmp(const void *ptr1, const void *ptr2, size_t n);
extern void *memchr(const void *ptr, int value, size_t n);
extern void *memset(void *ptr, int value, size_t n);

// string functions
extern size_t strlen(const char *str);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *dst, const char *src, size_t n);
extern char *strcat(char *s1, const char *s2);
extern char *strncat(char *s1, const char *s2, size_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strchr(const char *str, int value);
extern char *strstr(const char *s1, const char *s2);
extern unsigned int strtoul(const char *str, char **end, int base);
extern unsigned long long strtoull(const char *str, char **end, int base);

// stdio functions
// extern void sprintf(char *buf, const char *fmt, ...);
// extern int snprintf(char *buf, size_t n, const char *fmt, ...);

#endif // __CLIB_H__