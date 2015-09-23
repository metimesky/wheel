#ifndef __CPU_H__
#define __CPU_H__ 1

static inline void cpuid(int code, unsigned int* a, unsigned int* d) {
    __asm__ __volatile__("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ebx", "ecx");
}

static inline char* cpuid_vendor_string(int code, char* out) {
    __asm__ __volatile__("cpuid" : "=b"(*((int *) out)),
        "=d"(*((int *) out + 1)), "=c"(*((int *) out + 2)) : "a"(code));
    return out;
}

static inline unsigned long read_cr0(void) {
    unsigned long val;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline void invlpg(void* m) {
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    __asm__ __volatile__("invlpg (%0)" :: "b"(m) : "memory");
}

static inline unsigned long long read_msr(unsigned int msr_id) {
    unsigned long long msr_val;
    __asm__ __volatile__("rdmsr" : "=A"(msr_val) : "c"(msr_id));
    return msr_val;
}

static inline void write_msr(unsigned int msr_id, unsigned long long msr_val) {
    __asm__ __volatile__("wrmsr" :: "c"(msr_id), "A"(msr_val));
}

// read the CPU's time stamp value (pentium+), one of MSR
static inline unsigned long long rdtsc() {
    unsigned long long ret;
    __asm__ __volatile__("rdtsc" : "=A"(ret));
    return ret;
}

#endif // __CPU_H__