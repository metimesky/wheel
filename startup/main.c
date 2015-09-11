/* We need to make it a better name, or a better place
 */
#include <type.h>

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
    __asm__ volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

void print(const char *str, int pos) {
    static char *video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2*(pos+i)] = str[i];
        video[2*(pos+i)+1] = 0x0f;
    }
}

void kmain() {
    print("Welcome to Wheel Operating System", 0);
    uint32_t a, d;
    cpuid(1, &a, &d);
    if (d & (1UL << 9)) {
    	print("Yes, APIC support is on.", 80);
    }
    while (1) {}
}