/* We need to make it a better name, or a better place
 */
#include <type.h>
#include <util.h>
#include "fake_console.h"

void kernel_main() {
    print("Welcome to Wheel Operating System. ");
    uint32_t a, d;
    char buf[33];
    cpuid(0x80000008, &a, &d);
    print("phy_addr_sz: ");
    print(u32_to_str(a & 0xff, buf, 10));
    print(", lin_addr_sz: ");
    println(u32_to_str((a >> 8) & 0xff, buf, 10));
    // at this stage, identical paging is setup, GDT has only two segments for
    // kernel code and data. Interrupt still not initialized.

    if (multiprocessor_init()) {
        println("Seems your computer has only 1 CPU.");
    } else {
        println("You have a multi-processor computer!");
    }

    // check if the CPU has a built-in local APIC
    // uint32_t a, d;
    cpuid(1, &a, &d);
    if (d & (1UL << 9)) {
    	println("Yes, APIC support is on.");
    }
    while (1) {}
}