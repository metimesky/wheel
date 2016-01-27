#include "multiboot.h"
#include <utilities/env.h>
#include <utilities/clib.h>
#include <utilities/logging.h>

#include <memory/memory.h>
#include <interrupt/interrupt.h>

#include <drivers/console/console.h>
#include <drivers/acpi/acpi.h>
#include <drivers/apic/apic.h>
#include <drivers/pic/pic.h>
#include <drivers/pit/pit.h>
#include <drivers/hpet/hpet.h>

extern void goto_ring3(void *addr, void *rsp);
extern void delay();

char st[4096*3];

// this is the initial user-space application
void ps() {
    char *video = (char *) 0xb8000;
    for (int i = 0;; ++i) {
        video[2*i] = 'P';
        video[2*i+1] = 0x1e;
        //busy_wait(1000);
    }
    // this function has a flaw, it will overrun the video buffer and cause
    // serious consequencies. But for now, it is simple enough for testing.
}


/*******************************************************************************
 * This is the main function of the kernel, because kernel has no main loop.
 * When fully started, all kernel do is handling events and syscall.
 * This function is executed during system initialization.
 */
extern ap_init();

void init(uint32_t eax, uint32_t ebx) {
    // static allocator
    static_alloc_init();

    // initialize early 80x25 console support
    console_init();

    // check bootloader compliance
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        log("Bootloader not multiboot compliant!");
        return;
    }

    // early access ACPI tables, fail if not exist
    if (!initialize_acpi_tables()) {
        log("ACPI not available!");
        return;
    }

    // now the acpi has initialized, we know the number of cores
    // so we can switch GDT, with a TSS entry for each core
    log("statically allocated %x.", alloc_static(32));
    log("statically allocated %x.", alloc_static(17));
    log("statically allocated %x.", alloc_static(29));

    /* While allocating GDT and TSS is prefered using static_alloc, stack for
     * AP is better using page_alloc. So AP startup is splitted into two!!!
     */

    // initialize memory manager, so now we know the number of cpu
    memory_init((multiboot_info_t *) ebx);

    // initialize interrupt handling module
    interrupt_init();

    // initalize 8259 PIC, with all pins disabled
    pic_init();

    // start receiving interrupts, currently all disabled
    __asm__ __volatile__("sti");

    // intialize timmer, try HPET first, fallback to PIT
    if (!hpet_init()) {
        pit_init();
    }

    // we should check SMP or UP here
    if (!apic_init()) {
        log("No APIC support!");
        return;
    }

    log("Welcome to WHEEL OS [BSP]!");

    // start AP
    ap_init();

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