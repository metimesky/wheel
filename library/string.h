#ifndef __STRING_H__
#define __STRING_H__ 1

void memcpy(void *dst, void *src, int n);
void memmove(void *dst, void *src, int n);
void memset(void *dst, char c, int n);
// int memcmp(void *x, void *y, int n);
int memcmp(const void *s1, const void *s2, int n);

char *u32_to_str(unsigned int value, char *str, int base);
char *u64_to_str(unsigned long long value, char *str, int base);

#endif // __STRING_H__