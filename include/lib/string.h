#ifndef __STRING_H__
#define __STRING_H__

#include <wheel.h>

extern void *memcpy(void *dst, const void *src, size_t count);
extern void *memset(void *dst, int c, size_t count);
extern int memcmp(const void *ptr1, const void *ptr2, size_t count);

#endif