#ifndef __UTIL_H__
#define __UTIL_H__ 1

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile (
        "outb %0, %1"
        :: "a"(val), "Nd"(port)
    );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
}

static inline void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    __asm__ volatile (
        "outb %%al, $0x80"
        :: "a"(0)
    );
    /* TODO: Is there any reason why al is forced? */
}

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=d"(*d)
        : "0"(code)
        : "ebx", "ecx"
    );
}

// read the CPU's time stamp value (pentium+)
static inline uint64_t rdtsc() {
    uint64_t ret;
    __asm__ volatile (
        "rdtsc"
        : "=A"(ret)
    );
    return ret;
}

static inline unsigned long read_cr0(void) {
    unsigned long val;
    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(val)
    );
    return val;
}

static inline void invlpg(void* m) {
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    __asm__ volatile (
        "invlpg (%0)"
        :: "b"(m)
        : "memory"
    );
}

inline void wrmsr(uint32_t msr_id, uint64_t msr_value) {
    __asm__ volatile (
        "wrmsr"
        :: "c" (msr_id), "A" (msr_value)
    );
}

inline uint64_t rdmsr(uint32_t msr_id) {
    uint64_t msr_value;
    __asm__ volatile (
        "rdmsr"
        : "=A" (msr_value)
        : "c" (msr_id)
    );
    return msr_value;
}

#endif // __UTIL_H__