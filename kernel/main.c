    /* We need to make it a better name, or a better place
 */
#include <stdhdr.h>
#include <util.h>
#include "fake_console.h"
#include "../memory/paging.h"
#include "../memory/virt_alloc.h"
#include "../acpi/acpi.h"
#include "../acpi/madt.h"
#include <multiboot.h>
#include "interrupt.h"

void main(uint32_t eax, uint32_t ebx) {
    char buf[33];

    if (0x2badb002 != eax) {
        println("bootloader not multiboot compliant!");
        return;
    }
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
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

    // test interruption
    __asm__ __volatile__ ("sti");
    __asm__ __volatile__ ("ud2");
    while (1) {}
}