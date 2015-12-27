#include <common/stdhdr.h>
#include <common/multiboot.h>
#include <common/util.h>
#include <driver/console/console.h>
#include <core/interrupt/interrupt.h>
#include <driver/acpi/acpi.h>
#include <memory/memory.h>

#include <core/timming/timming.h>
#include <driver/pit/pit.h>

extern void goto_ring3(void *addr, void *rsp);
extern void delay();

char st[4096*3];

// this is the initial user-space application
void ps() {
    char *video = (char *) 0xb8000;
    for (int i = 0;; ++i) {
        video[2*i] = 'P';
        video[2*i+1] = 0x1e;
        busy_wait(1000);
    }
    // this function has a flaw, it will overrun the video buffer and cause
    // serious consequencies. But for now, it is simple enough for testing.
}

/* Initialize multi-processor
 */
void mp_init() {
    if (!madt_present) {
        log("Unlucy, no madt exist!");
        return;
    }

    // ensure no ipi pending
    while (* ((uint32_t *) (local_apic_base + 0x30)) & (1 << 12)) {}

    // send INIT IPI -- higher 32-bit half
    * ((uint32_t *) (local_apic_base + 0x310)) = (uint32_t) local_apic_ids[1] << 24;

    // send INIT IPI -- lower 32-bit half
    uint32_t control = (5UL << 8);
    * ((uint32_t *) (local_apic_base + 0x300)) = control;

    // wait 10ms (we have to implement PIT or HPET)

    // send SIPI (vector means the page index AP begin to execute code on)
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
        log("Bootloader not multiboot compliant!");
        return;
    }

    // retrieve multiboot info structure
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;

    // initialize memory management module
    memory_init(mbi);

    // at this stage, only support internal exception and 8259 irq
    // other vectors are registered later
    interrupt_init();
    
    // initialize acpi driver
    if (!acpi_init()) {
        log("ACPI not present!");
        return;
    }

    if (madt_present) {
        local_apic_init(local_apic_base);
    }


    pit_init();

    // test interruption
    // __asm__ __volatile__ ("sti");
    //__asm__ __volatile__ ("ud2");

    // uint64_t phy = alloc_pages(0);
    // map(0xffffffff00000000UL, phy);
    // goto_ring3(ps, 0xffffffff00000000UL);

    acpi_init();
    if (madt_present) {
        local_apic_init(local_apic_base);
    }

    while (1) {}

    /* Goto Ring3
     */
    uint64_t rsp = (uint64_t) &st;
    rsp += 4096 - 1;
    rsp &= ~(4096 - 1);
    goto_ring3(ps, rsp);

    while (1) {}
}