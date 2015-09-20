#ifndef __FAKE_CONSOLE__
#define __FAKE_CONSOLE__ 1

void print(const char *str);
void println(const char *str);
char *u32_to_str(unsigned int value, char *str, int base);
char *u64_to_str(unsigned long long value, char *str, int base);

#endif // __FAKE_CONSOLE__