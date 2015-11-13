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


// unsigned char in_byte(unsigned short port);
// unsigned short in_word(unsigned short port);
// unsigned int in_dword(unsigned short port);
// void out_byte(unsigned short port, unsigned char val);
// void out_word(unsigned short port, unsigned short val);
// void out_dword(unsigned short port, unsigned int val);
// void io_wait();

void load_idtr(void* idtr);

static inline void real_load_idtr(void* idtr) {
    __asm__ __volatile__("lidt (%0)" :: "a"(idtr));
}

#endif // __CPU_H__