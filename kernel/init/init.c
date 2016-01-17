#include "multiboot.h"
#include <utilities/env.h>
#include <utilities/clib.h>
#include <utilities/logging.h>

#include <memory/memory.h>
#include <interrupt/interrupt.h>
#include <timming/timming.h>

#include <drivers/console/console.h>
#include <drivers/acpi/acpi.h>
#include <drivers/apic/apic.h>
#include <drivers/pic/pic.h>
#include <drivers/pit/pit.h>

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
    // if (!madt_present) {
    //     log("Unlucy, no madt exist!");
    //     return;
    // }

    // ensure no ipi pending
    // while (* ((uint32_t *) (local_apic_base + 0x30)) & (1 << 12)) {}

    // send INIT IPI -- higher 32-bit half
    // * ((uint32_t *) (local_apic_base + 0x310)) = (uint32_t) local_apic_ids[1] << 24;

    // send INIT IPI -- lower 32-bit half
    // uint32_t control = (5UL << 8);
    // * ((uint32_t *) (local_apic_base + 0x300)) = control;

    // wait 10ms (we have to implement PIT or HPET)

    // send SIPI (vector means the page index AP begin to execute code on)
}

/*******************************************************************************
 * This is the main function of the kernel, because kernel has no main loop.
 * When fully started, all kernel do is handling events and syscall.
 * This function is executed during system initialization.
 */

extern void early_test();
extern void unwind();

void unwind_a() { unwind(); }
void unwind_b() { unwind_a(); }
void unwind_c() { unwind_b(); }

void init(uint32_t eax, uint32_t ebx) {
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        return;
    }

    // initialize console, currently only 80x25 is supported
    console_init();

    // initialize memory management module
    memory_init((multiboot_info_t *) ebx);

    // initialize interrupt handling module
    interrupt_init();

    // initalize 8259 PIC, with all pins disabled
    pic_init();

    // start receiving external interrupts, currently none
    __asm__ __volatile__("sti");

    // setup old 8253 pit and enable it
    pit_init();
    // pic_unmask(0);

    // early access ACPI tables
    if (!initialize_acpi_tables()) {
        log("ACPI not available!");
        return;
    }

    // we should check SMP or UP here
    if (!apic_init()) {
        // pic_init();
        // shouldn't be pic_init anymore, but we should unmask all pins.
    }

    // pic_unmask(0);
    log("Welcome to WHEEL OS!");

    while (1) {}

    // initialize acpi driver
    // if (!acpi_init()) {
    //     log("ACPI not present!");
    //     return;
    // }

    // if (madt_present) {
    //     local_apic_init(local_apic_base);
    // }


    pit_init();

    // test interruption
    // __asm__ __volatile__ ("sti");
    //__asm__ __volatile__ ("ud2");

    // uint64_t phy = alloc_pages(0);
    // map(0xffffffff00000000UL, phy);
    // goto_ring3(ps, 0xffffffff00000000UL);

    // acpi_init();
    // if (madt_present) {
    //     local_apic_init(local_apic_base);
    // }

    while (1) {}

////////////////////////////////////////////////////////////////////////////////
// Goto Ring3
    uint64_t rsp = (uint64_t) &st;
    rsp += 4096 - 1;
    rsp &= ~(4096 - 1);
    goto_ring3(ps, rsp);
////////////////////////////////////////////////////////////////////////////////

    while (1) {}
}