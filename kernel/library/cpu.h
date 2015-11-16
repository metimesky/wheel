#ifndef __CPU_H__
#define __CPU_H__ 1

#include <common/stdhdr.h>

static inline void cpuid(uint32_t code, uint32_t* a, uint32_t* d) {
    __asm__ __volatile__("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ebx", "ecx");
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

static inline void invlpg(void* m) {
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    __asm__ __volatile__("invlpg (%0)" :: "b"(m) : "memory");
}

static inline uint64_t read_msr(uint32_t msr_id) {
    uint64_t msr_val;
    __asm__ __volatile__("rdmsr" : "=A"(msr_val) : "c"(msr_id));
    return msr_val;
}

static inline void write_msr(uint32_t msr_id, uint64_t msr_val) {
    __asm__ __volatile__("wrmsr" :: "c"(msr_id), "A"(msr_val));
}

// read the CPU's time stamp value (pentium+), one of MSR
static inline uint64_t rdtsc() {
    uint64_t ret;
    __asm__ __volatile__("rdtsc" : "=A"(ret));
    return ret;
}

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

void load_gdtr(void* gdtr);
void load_idtr(void* idtr);
void load_tr(void* tr);

static inline void real_load_idtr(void* idtr) {
    __asm__ __volatile__("lidt (%0)" :: "a"(idtr));
}

#endif // __CPU_H__