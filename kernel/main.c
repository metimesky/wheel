/* We need to make it a better name, or a better place
 */
#include <type.h>
#include <util.h>
// #include <stdarg.h>

void print(const char *str, int pos) {
    static char *video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2*(pos+i)] = str[i];
        video[2*(pos+i)+1] = 0x0f;
    }
}

void kernel_main() {
    print("Welcome to Wheel Operating System", 0);
    // at this stage, identical paging is setup, GDT has only two segments for
    // kernel code and data. Interrupt still not initialized.

    if (multiprocessor_init()) {
        print("Seems your computer has only 1 CPU.", 80);
    } else {
        print("You have a multi-processor computer!", 80);
    }

    // check if the CPU has a built-in local APIC
    uint32_t a, d;
    cpuid(1, &a, &d);
    if (d & (1UL << 9)) {
    	print("Yes, APIC support is on.", 160);
    }
    while (1) {}
}