#include "acpi.h"
#include <drivers/console.h>
#include <utilities/string.h>

// ACPI本应为电源管理用，在这里只用来查询APIC和多核信息

static acpi_table_header_t *fadt_addr = NULL;
static acpi_table_header_t *ssdt_addr = NULL;
static acpi_table_madt_t *madt_addr = NULL;
static acpi_table_header_t *hpet_addr = NULL;

acpi_table_header_t *acpi_get_fadt() { return fadt_addr; }
acpi_table_header_t *acpi_get_ssdt() { return ssdt_addr; }
acpi_table_madt_t *acpi_get_madt() { return madt_addr; }
acpi_table_header_t *acpi_get_hpet() { return hpet_addr; }

static acpi_rsdp_t *find_rsdp() {
    // get the EBDA address (physical)
    uint16_t ebda_addr = * (uint16_t *) (KERNEL_VMA + 0x040e);
    const char *p = (const char *) ((uint64_t)ebda_addr << 4) + KERNEL_VMA;

    // 1) search the first KB of EBDA
    for (int i = 0; i < 1024/16; ++i) {
        if (strncmp(p, ACPI_SIG_RSDP, 8) == 0) {
            return (acpi_rsdp_t *) p;
        }
        p += 16;
    }

    // 2) Search 128K upper memory: E0000h-FFFFFh
    p = (const char *) (KERNEL_VMA + 0x000e0000);
    for (int i = 0; i < 128*1024/16; ++i) {
        if (strncmp(p, ACPI_SIG_RSDP, 8) == 0) {
            return (acpi_rsdp_t *) p;
        }
        p += 16;
    }

    return NULL;
}

void acpi_init() {
    acpi_rsdp_t *rsdp = find_rsdp();
    if (!rsdp) {
        // error, cannot find rsdp
        console_print("cannot find RSDP!\n");
        return;
    }
    
    console_print("found rsdp!\n");
    
    // parse rsdp
    if (rsdp->revision == 0) {
        // version 1
        acpi_table_rsdt_t *rsdt = (acpi_table_rsdt_t *) (KERNEL_VMA + rsdp->rsdt_physical_addr);
        if (strncmp(rsdt->header.signature, ACPI_SIG_RSDT, 4) == 0) {
            int num = (rsdt->header.length - sizeof(acpi_table_header_t)) / sizeof(uint32_t);
            for (int i = 0; i < num; ++i) {
                acpi_table_header_t *h = (acpi_table_header_t *) (KERNEL_VMA + rsdt->entries[i]);
                if (strncmp(h->signature, ACPI_SIG_FADT, 4) == 0) {
                    fadt_addr = (acpi_table_header_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_SSDT, 4) == 0) {
                    ssdt_addr = (acpi_table_header_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_MADT, 4) == 0) {
                    madt_addr = (acpi_table_madt_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_HPET, 4) == 0) {
                    hpet_addr = (acpi_table_header_t *) h;
                }
            }
        }
    } else if (rsdp->revision == 2) {
        // version 2
        acpi_table_xsdt_t *xsdt = (acpi_table_xsdt_t *) (KERNEL_VMA + rsdp->xsdt_physical_addr);
        if (strncmp(xsdt->header.signature, ACPI_SIG_XSDT, 4) == 0) {
            int num = (xsdt->header.length - sizeof(acpi_table_header_t)) / sizeof(uint64_t);
            for (int i = 0; i < num; ++i) {
                acpi_table_header_t *h = (acpi_table_header_t *) (KERNEL_VMA + xsdt->entries[i]);
                if (strncmp(h->signature, ACPI_SIG_FADT, 4) == 0) {
                    fadt_addr = (acpi_table_header_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_SSDT, 4) == 0) {
                    ssdt_addr = (acpi_table_header_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_MADT, 4) == 0) {
                    madt_addr = (acpi_table_madt_t *) h;
                } else if (strncmp(h->signature, ACPI_SIG_HPET, 4) == 0) {
                    hpet_addr = (acpi_table_header_t *) h;
                }
            }
        }
    }
}
