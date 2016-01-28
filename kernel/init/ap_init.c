// this file initialize all APs in an SMP environment.

#include <utilities/env.h>
#include <utilities/cpu.h>
#include <utilities/logging.h>
#include <drivers/apic/apic.h>
#include <scheduler/scheduler.h>
#include <memory/memory.h>

// AP startup trampoline code position (defined in link.lds)
extern char ap_boot_start;
extern char ap_boot_end;

volatile int ap_started;

// this function is called by `init`, executed on BSP
void ap_init() {
    ap_started = 0;

    // allocate new GDT and TSS for each core
    size_t n = 0;
    n += 5 * sizeof(uint64_t);  // GDT dummy, two code, two data
    n += local_apic_count * 2 * sizeof(uint64_t);   // tss entries in gdt
    // n += local_apic_count * sizeof(tss_t);

    // copy real mode trampoline code to 0x7c000
    memcpy(0x7c000, &ap_boot_start, &ap_boot_end - &ap_boot_start);

    // CMOS shutdown code = warm reset
    out_byte(0x70, 0x0f);   // address
    out_byte(0x71, 0x0a);   // value
    
    // set the warm reset vector = 0x7c00:0000
    * ((uint16_t *) 0x467) = 0x7c00;    // segment
    * ((uint16_t *) 0x469) = 0x0000;    // offset

    // start from index 1, skipping BSP
    for (int i = 1; i < local_apic_count; ++i) {
        log("starting local apic #%d, processor id %d", local_apic_arr[i]->Id);

        // first allocate a page as stack for AP.
        uint64_t addr = alloc_pages(0);

        // write the stack top address to 0x7cff8
        DATA_U64(0x7cff8) = addr + 0x1000;

        uint32_t icr_l, icr_h;

        local_apic_poll_error();

        icr_l = 0U;                             // vector number = 0
        icr_l |= (5U << 8) & 0x00000700U;       // delivery mode = INIT
        icr_l |= 0U << 11;                      // destination mode = physical
        icr_l |= 1U << 14;                      // assert
        icr_l |= 1U << 15;                      // trigger mode = level
        icr_l |= (0UL << 18) & 0x000c0000UL;    // destination shorthand = no
        icr_h = local_apic_arr[i]->Id;
        icr_h <<= 24;                           // target local apic id

        // send INIT IPI -- assert
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_1) = icr_h;
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_0) = icr_l;
        local_apic_send_ipi(icr_h, icr_l);

        // wait for 10 ms
        busy_wait(10);

        local_apic_poll_error();

        // icr_l &= ~(1U << 14);   // de-assert
        icr_l = 0U;                             // vector number = 0
        icr_l |= (5U << 8) & 0x00000700U;       // delivery mode = INIT
        icr_l |= 0U << 11;                      // destination mode = physical
        icr_l |= 0U << 14;                      // de-assert
        icr_l |= 1U << 15;                      // trigger mode = level
        icr_l |= (0UL << 18) & 0x000c0000UL;    // destination shorthand = no
        icr_h = local_apic_arr[i]->Id;
        icr_h <<= 24;                           // target local apic id

        // send INIT IPI --deassert
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_1) = icr_h;
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_0) = icr_l;
        local_apic_send_ipi(icr_h, icr_l);

        // wait for 10ms
        busy_wait(20);

        local_apic_poll_error();

        icr_l = 0x0000007cU;                    // segment = 0x7c00
        icr_l |= (6U << 8) & 0x00000700U;       // delivery mode = STARTUP
        icr_l |= 0U << 11;                      // destination mode = physical
        icr_l |= 1U << 14;                      // assert
        icr_l |= 1U << 15;                      // trigger mode = level
        icr_l |= (0UL << 18) & 0x000c0000UL;    // destination shorthand = no
        icr_h = local_apic_arr[i]->Id;
        icr_h <<= 24;                           // target local apic id

        // send STARTUP IPI, pass 1
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_1) = icr_h;
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_0) = icr_l;
        local_apic_send_ipi(icr_h, icr_l);

        // wait for 20us
        busy_wait(2);

        local_apic_poll_error();

        // send STARTUP IPI, pass 2
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_1) = icr_h;
        // DATA_U32(local_apic_base + INTERRUPT_COMMAND_0) = icr_l;
        local_apic_send_ipi(icr_h, icr_l);

        // wait for 20us
        busy_wait(2);
    }
}

// this function is executed on AP, once per AP
void ap_post_init() {
    // get the number of this AP.
    int id = ap_started;
    ++ap_started;

    // print the letter 'A'
    char *video = (char *) 0xb8000;
    video[2*id] = 'A';
    video[2*id+1] = 0x0c;

////////////////////////////////////////////////////////////////////////////////
    // first init interrupt, no need to fill idt, just load it
    // __asm__ ("lidt");

    // load TSS
    // __asm__ ("ltr");

    // initialize local apic, it's different from BSP
    local_apic_local_init();
    
    while (1) {}
}
