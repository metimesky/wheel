#include "acpi.h"
#include <type.h>
#include <util.h>
#include "../kernel/fake_console.h"

uint64_t find_rsdp() {
    uint64_t ebda_addr = *((uint16_t *) 0x40e) << 4;

    uint64_t sig = 0UL;
    // sig |= ((uint64_t) 'R' <<  0) & 0x00000000000000ff;
    // sig |= ((uint64_t) 'S' <<  8) & 0x000000000000ff00;
    // sig |= ((uint64_t) 'D' << 16) & 0x0000000000ff0000;
    // sig |= ((uint64_t) ' ' << 24) & 0x00000000ff000000;
    // sig |= ((uint64_t) 'P' << 32) & 0x000000ff00000000;
    // sig |= ((uint64_t) 'T' << 40) & 0x0000ff0000000000;
    // sig |= ((uint64_t) 'R' << 48) & 0x00ff000000000000;
    // sig |= ((uint64_t) ' ' << 56) & 0xff00000000000000;
    memcpy(&sig, "RSD PTR ", 8);

    // 1. search for RSDP in first KB of EBDA
    uint64_t *addr = (uint64_t *) ebda_addr;
    // for (uint64_t *addr = (uint64_t *) ebda_addr; addr < ebda_addr + 1024; addr += 2) {
    for (int i = 0; i < 128; i += 2) {
        if (sig == addr[i]) {
            return &addr[i];
        }
    }

    // 2. search for RSDP in highest base mem
    for (uint64_t *addr = (uint64_t *) 0xe0000; addr < 0x100000; addr += 2) {
        if (sig == *addr) {
            return addr;
        }
    }

    return 0;
}

int is_rsdp_1_valid(rsdp_1_t *rsdp) {
    uint8_t *p = rsdp;
    uint8_t sum = 0;
    for (int i = 0; i < sizeof(rsdp_1_t); ++i) {
        sum += p[i];
    }
    return sum == 0;
}

int is_rsdp_2_valid(rsdp_2_t *rsdp) {
    if (is_rsdp_1_valid(&(rsdp->rsdp_1))) {
        uint8_t *p = &(rsdp->length);
        uint8_t sum = 0;
        for (int i = 0; i < rsdp->length - sizeof(rsdp_1_t); ++i) {
            sum += p[i];
        }
        return sum == 0;
    }
    return 0;
}

// check the validness of all kinds of SDT
int is_sdt_valid(sdt_header_t *sdt) {
    uint8_t *p = (uint8_t *) sdt;
    uint8_t sum = 0;
    for (int i = 0; i < sdt->length; ++i) {
        sum += p[i];
    }
    return sum == 0;
}

void process_madt(madt_t *madt) {
    if (!is_sdt_valid(madt)) {
        println("ACPI::MADT not valid!");
        return;
    }
    print("apic local controller addr is 0x");
    char buf[33];
    print(u32_to_str(madt->local_controller_addr, buf, 16));
    if (madt->flags == 1) {
        println(" with dual 8259 legacy.");
    } else {
        println(" without dual 8259 legacy.");
    }
    // return;
    uint8_t *p = madt->entries;
    int i = 0;
    while (p < (uint8_t *) madt + madt->header.length/* && i < 30*/) {
        switch (*p) {
        case 0: {   // processor local APIC
            madt_local_apic_entry_t *entry = (madt_local_apic_entry_t *) p;
            // print("processor local APIC, length=");
            // print(u32_to_str(entry->length, buf, 10));
            // print(" and size=");
            // print(u32_to_str(sizeof(madt_local_apic_entry_t), buf, 10));
            // print(". processorId ");
            // print(u32_to_str(entry->processor_id, buf, 10));
            // print(" APICId ");
            // print(u32_to_str(entry->local_apic_id, buf, 10));
            // print(" flag ");
            // println(u32_to_str(entry->flags, buf, 10));
            p += entry->length;
        }
            break;
        case 1: {   // IO APIC
            madt_io_apic_entry_t *entry = (madt_io_apic_entry_t *) p;
            // print("IO APIC, length=");
            // print(u32_to_str(entry->length, buf, 10));
            // print(" and size=");
            // print(u32_to_str(sizeof(madt_io_apic_entry_t), buf, 10));
            // print(". APIC id=");
            // print(u32_to_str(entry->io_apic_id, buf, 10));
            // print(" addr=0x");
            // print(u32_to_str(entry->io_apic_addr, buf, 16));
            // print(" int_base=0x");
            // println(u32_to_str(entry->global_system_interrupt_base, buf, 16));
            p += entry->length;
        }
            break;
        case 2: {   // interrupt source override
            madt_interrupt_src_override_t *entry = (madt_interrupt_src_override_t *) p;
            // println("interrupt source override");
            // print("int from bus ");
            // print(u32_to_str(entry->bus_source, buf, 10));
            // print(" irq ");
            // print(u32_to_str(entry->irq_source, buf, 16));
            // print(" g ");
            // println(u32_to_str(entry->global_system_interrupt, buf, 16));
            p += entry->length;
        }
            break;
        default:
            // println("unknown MADT entry type!");
            ++p;
            break;
        }
        ++i;
    }
}

// for each entry in the RSDT or XSDT, check it's type
// and dispatch the process job to individual function
void process_acpi_table(uint64_t addr) {
    if (memcmp((char *) addr, "APIC", 4) == 0) {
        // multiple APIC description table
        // println("APIC");
        process_madt((madt_t *) addr);
    } else if (memcmp((char *) addr, "BERT", 4) == 0) {
        // boot error record table
        println("BERT");
    } else if (memcmp((char *) addr, "CPEP", 4) == 0) {
        // corrected platform error polling table
        println("CPEP");
    } else if (memcmp((char *) addr, "DSDT", 4) == 0) {
        // differentiated system description table
        println("DSDT");
    } else if (memcmp((char *) addr, "ECDT", 4) == 0) {
        // embedded controller boot resources table
        println("ECDT");
    } else if (memcmp((char *) addr, "EINJ", 4) == 0) {
        // error injection table
        println("EINJ");
    } else if (memcmp((char *) addr, "ERST", 4) == 0) {
        // error record serialization table
        println("ERST");
    } else if (memcmp((char *) addr, "FACP", 4) == 0) {
        // fixed ACPI description table
        println("FACP");
    } else if (memcmp((char *) addr, "FACS", 4) == 0) {
        // firmware ACPPI control structure
        println("FACS");
    } else if (memcmp((char *) addr, "HEST", 4) == 0) {
        // hardware error source table
        println("HEST");
    } else if (memcmp((char *) addr, "MSCT", 4) == 0) {
        // maximum system characteristics table
        println("MSCT");
    } else if (memcmp((char *) addr, "MPST", 4) == 0) {
        // memory power state table
        println("MPST");
    } else if (memcmp((char *) addr, "OEM", 3) == 0) {
        // OEM specific information tables
        // there is not one table, but many tables!
        println("OEM");
    } else if (memcmp((char *) addr, "PMTT", 4) == 0) {
        // platform memory topology table
        println("PMTT");
    } else if (memcmp((char *) addr, "PSDT", 4) == 0) {
        // persistent system description table
        println("PSDT");
    } else if (memcmp((char *) addr, "RASF", 4) == 0) {
        // ACPI RAS feature table
        println("RASF");
    } else if (memcmp((char *) addr, "RSDT", 4) == 0) {
        // RSDT, already processed
        println("RSDT");
    } else if (memcmp((char *) addr, "SBST", 4) == 0) {
        // smart battery specification table
        println("SBST");
    } else if (memcmp((char *) addr, "SLIT", 4) == 0) {
        // System Locality System Information Table
        println("SLIT");
    } else if (memcmp((char *) addr, "SRAT", 4) == 0) {
        // System Resource Affinity Table
        println("SRAT");
    } else if (memcmp((char *) addr, "SSDT", 4) == 0) {
        // Secondary System Description Table
        println("SSDT");
    } else if (memcmp((char *) addr, "XSDT", 4) == 0) {
        // XSDT, already processed
        println("XSDT");
    } else {
        print("Others ");
        char buf[5];
        memcpy(buf, (char *) addr, 4);
        buf[4] = '\0';
        println(buf);
    }
}

void acpi_init() {
    char buf[33];
    uint64_t addr = find_rsdp();
    if (!addr) {
        println("This computer has no ACPI!");
        return;
    }

    // get version ACPI version
    if (((rsdp_1_t *) addr)->revision == 0) {
        // revision = 0 means version 1.0
        rsdp_1_t *rsdp = (rsdp_1_t *) addr;
        // println(rsdp->signature);
        if (!is_rsdp_1_valid(rsdp)) {
            println("RSDP 1.0 is not valid.");
            return;
        }
        // get RSDT from RSDP
        rsdt_t *rsdt = (rsdt_t *) (rsdp->rsdt_addr);
        // validate RSDT
        if (memcmp(rsdt->header.signature, "RSDT", 4) != 0 || !is_sdt_valid(&(rsdt->header))) {
            println("ACPI::RSDT not valid.");
            return;
        }
        int sdt_num = (rsdt->header.length - sizeof(rsdt->header)) / sizeof(uint32_t);
        for (int i = 0; i < sdt_num; ++i) {
            process_acpi_table(rsdt->sdt_entries[i]);
        }
        print("done");
        println(u32_to_str(sdt_num, buf, 10));
    } else {
        // newer than 1.0, treat them as 2.0
        rsdp_2_t *rsdp = (rsdp_2_t *) addr;
        if (!is_rsdp_2_valid(rsdp)) {
            println("RSDP 2.0 is not valid.");
            return;
        }
        // get XSDT from RSDP
        xsdt_t *xsdt = (xsdt_t *) (rsdp->xsdt_addr);
        // validate XSDT
        if (memcmp(xsdt->header.signature, "XSDT", 4) != 0 || !is_sdt_valid(&(xsdt->header))) {
            println("ACPI::XSDT not valid.");
            return;
        }
        int sdt_num = (xsdt->header.length - sizeof(xsdt->header)) / sizeof(uint64_t);
        for (int i = 0; i < sdt_num; ++i) {
            process_acpi_table(xsdt->sdt_entries[i]);
        }
        println("done");
    }
}