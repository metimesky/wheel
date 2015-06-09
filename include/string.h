#ifndef __STRING_H__
#define __STRING_H__ 1

int strlen(const char* s);

char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, int n);

char* u32_to_str(unsigned int value, char *str, int base);
char* u64_to_str(unsigned long long value, char *str, int base);
char* i32_to_str(int value, char *str, int base);
char* i64_to_str(long long value, char *str, int base);
/*
int sprintf(char *s, const char *fmt, ...);
int snprintf(char *s, int n, const char *fmt, ...);
*/
#endif // __STRING_H__