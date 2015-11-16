    /* We need to make it a better name, or a better place
 */
#include <stdhdr.h>
#include <util.h>
#include "fake_console.h"
#include "../memory/paging.h"
#include "../memory/virt_alloc.h"
#include "../acpi/acpi.h"
#include "../acpi/madt.h"
#include "../dev/console/console.h"
#include <multiboot.h>
#include "interrupt.h"

void print_junk(char x) {
    console_put_char(x);
    console_put_string("hello, world! 1\n");
    console_put_string("hello, world! 2\n");
    console_put_string("hello, world! 3\n");
    console_put_string("hello, world! 4\n");
    console_put_string("hello, world! 5\n");
    console_put_string("hello, world! 6\n");
    console_put_string("hello, world! 7\n");
    console_put_string("hello, world! 8\n");
    console_put_string("hello, world! 9\n");
    console_put_string("hello, world! 0\n");
}

void main(uint32_t eax, uint32_t ebx) {
    char buf[33];

    // initialize early console to print verbose information.
    // currently only 80x25 text mode is supported, graphics mode may be
    // turned on later.
    console_init();

    // check if magic number is compliant
    if (0x2badb002 != eax) {
        println("bootloader not multiboot compliant!");
        return;
    }

    // retrieve multiboot info structure
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;

    // initialize memory management module
    memory_init(mbi);

    // TODO: in future i want to do as:
    //     memory_init(mbi);
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);

    // init ACPI
    // TODO: currently we use custom code that only finds MADT. In the lone term
    // we should switch to ACPICA and call its init function
    acpi_init();

    // check if madt was found during ACPI init
    if (!madt_present) {
        // if not, try searching for MP tables
        multiprocessor_init();
    }

    // TODO: initialize interrupt (stuffs like IDT, ISR, etc)
    // we have to find APIC table from ACPI (WTF!)
    interrupt_init();

    cpuid_vendor_string(0, buf);
    buf[12] = '\0';
    printf("CPU Vendor: %s\n", buf);

    // console_scroll(5);
    console_set_cursor(0, 7);

    // test interruption
    // __asm__ __volatile__ ("sti");
    // __asm__ __volatile__ ("ud2");

    console_init();

    for (int i = 0; i < 20; ++i) {
        print_junk('a'+i);
        for (int j = 0; j < 1000; ++j) { for (int k = 0; k < 100; ++k) { for (int o = 0; o < 100; ++o) { for (int p = 0; p < 100; ++p) {} } } }
    }
    while (1) {}
}