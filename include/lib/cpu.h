#ifndef __CPU_H__
#define __CPU_H__

#include <wheel.h>

static inline void cpuid(uint32_t code, uint32_t* a, uint32_t *b, uint32_t *c, uint32_t* d) {
    __asm__ __volatile__("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(code), "b"(*b), "c"(*c), "d"(*d));
}

static inline char* cpuid_vendor_string(uint32_t code, char* out) {
    __asm__ __volatile__("cpuid" : "=b"(*((int *) out)),
        "=d"(*((int *) out + 1)), "=c"(*((int *) out + 2)) : "a"(code));
    return out;
}

static inline uint64_t read_cr0() {
    uint64_t val;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline uint64_t read_cr3() {
    uint64_t val;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void invlpg(void* m) {
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    __asm__ __volatile__("invlpg (%0)" :: "b"(m) : "memory");
}

// directive `A` means rdx:rax in 64-bit mode
static inline uint64_t read_msr(uint32_t msr_id) {
    union {
        uint32_t d[2];
        uint64_t q;
    } u;
    __asm__ __volatile__("rdmsr" : "=d"(u.d[1]), "=a"(u.d[0]) : "c"(msr_id));

    return u.q;
}

static inline void write_msr(uint32_t msr_id, uint64_t msr_val) {
    union {
        uint32_t d[2];
        uint64_t q;
    } u;
    u.q = msr_val;
    __asm__ __volatile__("wrmsr" :: "d"(u.d[1]), "a"(u.d[0]), "c"(msr_id));
}

static inline uint64_t read_fsbase() { return read_msr(0xc0000100); }
static inline void write_fsbase(uint64_t val) { write_msr(0xc0000100, val); }

static inline uint64_t read_gsbase() { return read_msr(0xc0000101); }
static inline void write_gsbase(uint64_t val) { write_msr(0xc0000101, val); }

// read the CPU's time stamp value (pentium+), one of MSR
static inline uint64_t rdtsc() {
    uint64_t ret;
    __asm__ __volatile__("rdtsc" : "=A"(ret));
    return ret;
}

// 端口操作函数

static inline uint8_t in_byte(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t in_word(uint16_t port) {
    uint16_t ret;
    __asm__ __volatile__("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t in_dword(uint16_t port) {
    uint32_t ret;
    __asm__ __volatile__("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void out_byte(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline void out_word(uint16_t port, uint16_t val) {
    __asm__ __volatile__("outw %0, %1" :: "a"(val), "Nd"(port));
}

static inline void out_dword(uint16_t port, uint32_t val) {
    __asm__ __volatile__("outl %0, %1" :: "a"(val), "Nd"(port));
}

static inline void io_wait() {
    __asm__ __volatile__("outb %%al, $0x80" :: "a"(0));
}

#endif