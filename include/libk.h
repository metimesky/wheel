#ifndef LIBK_H_
#define LIBK_H_ 1

void* memcpy(void *dst, void *src, size_t n);
void* memset(void *buf, unsigned char byte, size_t n);

char* uint_to_str(unsigned int value, char *str, int base);
char* int_to_str(int value, char *buf, int base);

#endif // LIBK_H_
