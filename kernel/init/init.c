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

#include "gdt_tss.h"

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

// structure of long mode TSS
struct tss {
    uint32_t reserved_1;
    uint32_t rsp0_l;
    uint32_t rsp0_h;
    uint32_t rsp1_l;
    uint32_t rsp1_h;
    uint32_t rsp2_l;
    uint32_t rsp2_h;
    uint32_t reserved_2;
    uint32_t reserved_3;
    uint32_t ist1_l;
    uint32_t ist1_h;
    uint32_t ist2_l;
    uint32_t ist2_h;
    uint32_t ist3_l;
    uint32_t ist3_h;
    uint32_t ist4_l;
    uint32_t ist4_h;
    uint32_t ist5_l;
    uint32_t ist5_h;
    uint32_t ist6_l;
    uint32_t ist6_h;
    uint32_t ist7_l;
    uint32_t ist7_h;
    uint32_t reserved_4;
    uint32_t reserved_5;
    uint16_t reserved_6;
    uint16_t io_map_base;
} __attribute__((packed));
typedef struct tss tss_t;

// switch to new gdt and tss, apic needs to be initialized
void gdt_tss_init() {
    ;
}

void smp_init() {
    // first check the number of processor
    apic_init()
}

void init(uint32_t eax, uint32_t ebx) {
    // initialize primitive static memory allocator.
    static_alloc_init();

    // initialize early 80x25 console support, so we can `log` now.
    console_init();

    // check bootloader compliance
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        log("Bootloader not multiboot compliant!");
        return;
    }

    // initialize memory manager, so now we know the number of cpu
    // memory_init((multiboot_info_t *) ebx);

    // early access ACPI tables, fail if not exist.
    if (!initialize_acpi_tables()) {
        log("ACPI not available!");
        return;
    }

    // once ACPI is initialized, we can access SMP info from MADT.
    smp_init();

    // now the acpi has initialized, we know the number of cores
    // so we can switch GDT, with a TSS entry for each core.
    // New GDT and TSSs are packed into pages, we have to calculate the size.
    log("Core number is %d", local_apic_count);
    int gdt_size = 5 * 8; // gdt -- dummy + code0 + data0 + code3 + data3
    gdt_size += local_apic_count * 16;    // gdt tss entry
    uint64_t *new_gdt = alloc_static(gdt_size);

    for (int i = 0; i < local_apic_count; ++i) {
        tss_t *new_tss = (tss_t *) alloc_static(sizeof(tss_t));
    }

    /* While allocating GDT and TSS is prefered using static_alloc, stack for
     * AP is better using page_alloc. So AP startup is splitted into two!!!
     */

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