    /* We need to make it a better name, or a better place
 */
#include <common/stdhdr.h>
#include <common/multiboot.h>
#include <common/util.h>
#include <driver/console/console.h>
#include <driver/acpi/acpi.h>
#include <memory/memory.h>
#include <interrupt/interrupt.h>

/**
    This is the main function of the kernel, because kernel has no main loop.
    When fully started, all kernel do is handling events and syscall.
    This function is executed during system initialization.
 */
void init(uint32_t eax, uint32_t ebx) {
    // initialize early console to print verbose information.
    // currently only 80x25 text mode is supported, graphics mode may be
    // turned on later.
    console_init();

    // check if magic number is compliant
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        // println("bootloader not multiboot compliant!");
        return;
    }

    // retrieve multiboot info structure
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;

    // initialize memory management module
    memory_init(mbi);

    // console_put_char('A');
    // console_put_string("this is string!\n");
    log("hello, world!");
    log("This operating system is called Wheel!");
    log("Now testing %s.", "formatted string");
    log("magic is 0x%x.", eax);
    log("this %s test %checks %d features.", "one", 'c', 3);

    interrupt_init();

/*
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
*/

    // test interruption
    // __asm__ __volatile__ ("sti");
    // __asm__ __volatile__ ("ud2");

    while (1) {}
}