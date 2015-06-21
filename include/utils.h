#ifndef __UTILS_H__
#define __UTILS_H__ 1

static inline void panic(char *msg) {
    static char *video = (char*) 0xb8000;
    static char attr = 0x4e;
    for (int i = 0; msg[i]; ++i) {
        video[2*i] = msg[i];
        video[2*i+1] = attr;
    }
    while (1) {}
}

#define str(s) #s
#define mstr(s) str(s)

#define assert(cond) if (!(cond)) panic("ASSERT FAIL: " __FILE__ "(" mstr(__LINE__) "): \"" #cond "\"")

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
    __asm__ volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

#endif // __UTILS_H__
