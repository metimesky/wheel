#include "multiboot.h"
#include <utilities/env.h>
#include <utilities/clib.h>
#include <utilities/logging.h>

#include <memory/memory.h>
#include <interrupt/interrupt.h>
#include <timing/timing.h>

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

extern void local_apic_start_ap();
extern char trampoline;
extern char trampoline_end;

void init(uint32_t eax, uint32_t ebx) {
    // initialize early 80x25 console support
    console_init();

    // check bootloader compliance
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        log("Bootloader not multiboot compliant!");
        return;
    }

    // initialize memory manager
    memory_init((multiboot_info_t *) ebx);

    // early access ACPI tables, fail if not exist
    if (!initialize_acpi_tables()) {
        log("ACPI not available!");
        return;
    }

    // initialize interrupt handling module
    interrupt_init();

    // initalize 8259 PIC, with all pins disabled
    pic_init();

    // start receiving external interrupts, currently none
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

    log("Welcome to WHEEL OS!");

    // copy real mode startup code to 0x7c000
    log("copying");
    char code[] = "\xb8\x00\xb8\x8e\xe8\xb0\x58\xb4\x1e\x65\xa3\x06\x00\xeb\xfe\xeb\xfe";
    char *src = &trampoline;
    char *dst = (char *) 0x7c000;
    int n = &trampoline_end - &trampoline;
    for (int i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
    // for (int i = 0; i < 17; ++i) {
    //     dst[i] = code[i];
    // }

    // start AP
    local_apic_start_ap();

    while (1) {}

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