#ifndef __UTIL_H__
#define __UTIL_H__ 1

#include <type.h>

static inline uint8_t in_byte(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t in_word(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t in_dword(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void out_byte(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    __asm__ volatile ("outb %%al, $0x80" :: "a"(0));
    /* TODO: Is there any reason why al is forced? */
}

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
    __asm__ volatile ("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

// read the CPU's time stamp value (pentium+)
static inline uint64_t rdtsc() {
    uint64_t ret;
    __asm__ volatile ("rdtsc" : "=A"(ret));
    return ret;
}

static inline unsigned long read_cr0(void) {
    unsigned long val;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline void invlpg(void* m) {
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    __asm__ volatile ("invlpg (%0)" :: "b"(m) : "memory");
}

static inline uint64_t read_msr(uint32_t msr_id) {
    uint64_t msr_val;
    __asm__ __volatile__("rdmsr" : "=A"(msr_val) : "c"(msr_id));
    return msr_val;
}

static inline void write_msr(uint32_t msr_id, uint64_t msr_val) {
    __asm__ __volatile__("wrmsr" :: "c"(msr_id), "A"(msr_val));
}

// static inline void read_msr(uint32_t msr, uint32_t *lo, uint32_t *hi) {
//    __asm__ volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
// }
 
// static inline void write_msr(uint32_t msr, uint32_t lo, uint32_t hi) {
//    __asm__ volatile("wrmsr" :: "a"(lo), "d"(hi), "c"(msr));
// }

#endif // __UTIL_H__