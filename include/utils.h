#ifndef __UTILS_H__
#define __UTILS_H__ 1

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
    asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

#endif // __UTILS_H__
