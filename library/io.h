#ifndef __IO_H__
#define __IO_H__ 1

static inline unsigned char in_byte(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline unsigned short in_word(unsigned short port) {
    unsigned short ret;
    __asm__ __volatile__("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline unsigned int in_dword(unsigned short port) {
    unsigned int ret;
    __asm__ __volatile__("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void out_byte(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline void out_word(unsigned short port, unsigned short val) {
    __asm__ __volatile__("outw %0, %1" :: "a"(val), "Nd"(port));
}

static inline void out_dword(unsigned short port, unsigned int val) {
    __asm__ __volatile__("outl %0, %1" :: "a"(val), "Nd"(port));
}

static inline void io_wait() {
    __asm__ __volatile__("outb %%al, $0x80" :: "a"(0));
}

#endif // __IO_H__