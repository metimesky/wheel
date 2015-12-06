    /* We need to make it a better name, or a better place
 */
#include <common/stdhdr.h>
#include <common/multiboot.h>
#include <common/util.h>
#include <driver/console/console.h>
#include <driver/acpi/acpi.h>
#include <memory/memory.h>
#include <interrupt/interrupt.h>

#include <memory/memory.h>

extern void goto_ring3(void *addr, void *rsp);
extern void delay();

char st[4096*3];

// this is the initial user-space application
void ps() {
    char *video = (char *) 0xb8000;
    for (int i = 0;; ++i) {
        video[2*i] = 'P';
        video[2*i+1] = 0x1e;
        for (int a = 0; a < 1000; ++a) {
            for (int b = 0; b < 2000; ++b) {
                delay();    // since we enabled optimization, we have to
                            // call a function to delay, or compiler would
                            // generate nothing.
            }
        }
    }
    // this function has a flaw, it will overrun the video buffer and cause
    // serious consequencies. But for now, it is simple enough for testing.
}

/*******************************************************************************
 * This is the main function of the kernel, because kernel has no main loop.
 * When fully started, all kernel do is handling events and syscall.
 * This function is executed during system initialization.
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
    log("free page count = %d.", free_page_count);

    interrupt_init();

    uint64_t *p0 = slab_alloc(sizeof(uint64_t));
    log("allocated at %p.", p0);
    uint64_t *p1 = slab_alloc(sizeof(uint64_t));
    log("allocated at %p.", p1);
    uint64_t *p2 = slab_alloc(sizeof(uint64_t));
    log("allocated at %p.", p2);

    slab_free(p0);
    log("freeing %p.", p0);
    p0 = slab_alloc(sizeof(uint64_t));
    log("allocated at %p.", p0);

    slab_free(p2);
    log("freeing %p.", p2);
    p2 = slab_alloc(sizeof(uint64_t));
    log("allocated at %p.", p2);

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
    //__asm__ __volatile__ ("ud2");

    // uint64_t phy = alloc_pages(0);
    // map(0xffffffff00000000UL, phy);
    // goto_ring3(ps, 0xffffffff00000000UL);

    uint64_t rsp = (uint64_t) &st;
    rsp += 4096 - 1;
    rsp &= ~(4096 - 1);
    goto_ring3(ps, rsp);

    while (1) {}
}