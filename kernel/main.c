    /* We need to make it a better name, or a better place
 */
#include <stdhdr.h>
#include <util.h>
#include "fake_console.h"
#include "../acpi/madt.h"
#include <multiboot.h>

void acpi_init();

void parse_mb_info(multiboot_info_t *mbi) {
    ;
}

void main(uint32_t eax, uint32_t ebx) {
    char buf[33];

    if (0x2badb002 != eax) {
        println("bootloader not multiboot compliant!");
        return;
    }
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
    // TODO: init physical memory management buddy using mbi.
    // allocator_init(mbi);

    // init ACPI
    // TODO: currently we use custom code that only finds MADT. In the lone term
    // we should switch to ACPICA and call its init function
    acpi_init();

    // check if madt was found during ACPI init
    if (!madt_present) {
        // if not, try searching for MP tables
        multiprocessor_init();
    }

    cpuid_vendor_string(0, buf);
    buf[12] = '\0';
    print("CPU Vendor: ");
    println(buf);
    while (1) {}
}