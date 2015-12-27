#include <common/stdhdr.h>
#include <common/util.h>
#include "acpi.h"
#include "madt.h"
#include <driver/hpet/hpet.h>

extern void process_madt(madt_t *madt);

uint64_t find_rsdp() {
    uint64_t ebda_addr = * ((uint16_t *) 0x40e) << 4;

    char *sig_str = "RSD PTR ";     // notice there's a space at the end
    uint64_t sig = * ((uint64_t *) sig_str);
    // uint64_t sig = 0UL;
    // memcpy(&sig, "RSD PTR ", 8);

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

// check if RSDP version 1 is valid
bool is_rsdp_1_valid(rsdp_1_t *rsdp) {
    uint8_t *p = rsdp;
    uint8_t sum = 0;
    for (int i = 0; i < sizeof(rsdp_1_t); ++i) {
        sum += p[i];
    }
    return sum == 0;
}

// check if RSDP version 2 is valid
bool is_rsdp_2_valid(rsdp_2_t *rsdp) {
    if (is_rsdp_1_valid(&(rsdp->rsdp_1))) {
        uint8_t *p = &(rsdp->length);
        uint8_t sum = 0;
        for (int i = 0; i < rsdp->length - sizeof(rsdp_1_t); ++i) {
            sum += p[i];
        }
        return sum == 0;
    }
    return false;
}

// check the validness of all kinds of SDT
bool is_sdt_valid(sdt_header_t *sdt) {
    uint8_t *p = (uint8_t *) sdt;
    uint8_t sum = 0;
    for (int i = 0; i < sdt->length; ++i) {
        sum += p[i];
    }
    return sum == 0;
}

// for each entry in the RSDT or XSDT, check it's type
// and dispatch the process job to individual function
void process_acpi_table(uint64_t addr) {
    if (memcmp((char *) addr, "APIC", 4) == 0) {
        // multiple APIC description table
        // log("APIC");
        process_madt((madt_t *) addr);
    } else if (memcmp((char *) addr, "BERT", 4) == 0) {
        // boot error record table
        // log("BERT");
    } else if (memcmp((char *) addr, "CPEP", 4) == 0) {
        // corrected platform error polling table
        // log("CPEP");
    } else if (memcmp((char *) addr, "DSDT", 4) == 0) {
        // differentiated system description table
        // log("DSDT");
    } else if (memcmp((char *) addr, "ECDT", 4) == 0) {
        // embedded controller boot resources table
        // log("ECDT");
    } else if (memcmp((char *) addr, "EINJ", 4) == 0) {
        // error injection table
        // log("EINJ");
    } else if (memcmp((char *) addr, "ERST", 4) == 0) {
        // error record serialization table
        // log("ERST");
    } else if (memcmp((char *) addr, "FACP", 4) == 0) {
        // fixed ACPI description table
        // log("FACP");
    } else if (memcmp((char *) addr, "FACS", 4) == 0) {
        // firmware ACPPI control structure
        // log("FACS");
    } else if (memcmp((char *) addr, "HEST", 4) == 0) {
        // hardware error source table
        // log("HEST");
    } else if (memcmp((char *) addr, "HPET", 4) == 0) {
        // High Precision Event Timer
        // log("HPET");
        process_hpet((hpet_t *) addr);
    } else if (memcmp((char *) addr, "MSCT", 4) == 0) {
        // maximum system characteristics table
        // log("MSCT");
    } else if (memcmp((char *) addr, "MPST", 4) == 0) {
        // memory power state table
        // log("MPST");
    } else if (memcmp((char *) addr, "OEM", 3) == 0) {
        // OEM specific information tables
        // there is not one table, but many tables!
        // log("OEM");
    } else if (memcmp((char *) addr, "PMTT", 4) == 0) {
        // platform memory topology table
        // log("PMTT");
    } else if (memcmp((char *) addr, "PSDT", 4) == 0) {
        // persistent system description table
        // log("PSDT");
    } else if (memcmp((char *) addr, "RASF", 4) == 0) {
        // ACPI RAS feature table
        // log("RASF");
    } else if (memcmp((char *) addr, "RSDT", 4) == 0) {
        // RSDT, already processed
        // log("RSDT");
    } else if (memcmp((char *) addr, "SBST", 4) == 0) {
        // smart battery specification table
        // log("SBST");
    } else if (memcmp((char *) addr, "SLIT", 4) == 0) {
        // System Locality System Information Table
        // log("SLIT");
    } else if (memcmp((char *) addr, "SRAT", 4) == 0) {
        // System Resource Affinity Table
        // log("SRAT");
    } else if (memcmp((char *) addr, "SSDT", 4) == 0) {
        // Secondary System Description Table
        // log("SSDT");
    } else if (memcmp((char *) addr, "XSDT", 4) == 0) {
        // XSDT, already processed
        // log("XSDT");
    } else {
        // log("WTF!");
    }
}

bool acpi_init() {
    uint64_t addr = find_rsdp();

    if (0 == addr) {
        return false;
    }

    // get version ACPI version
    if (((rsdp_1_t *) addr)->revision == 0) {
        // revision = 0 means version 1.0
        rsdp_1_t *rsdp = (rsdp_1_t *) addr;
        // log(rsdp->signature);
        if (!is_rsdp_1_valid(rsdp)) {
            log("RSDP 1.0 is not valid.");
            return false;
        }
        // get RSDT from RSDP
        rsdt_t *rsdt = (rsdt_t *) (rsdp->rsdt_addr);
        // validate RSDT
        if (memcmp(rsdt->header.signature, "RSDT", 4) != 0 || !is_sdt_valid(&(rsdt->header))) {
            log("ACPI::RSDT not valid.");
            return false;
        }
        int sdt_num = (rsdt->header.length - sizeof(rsdt->header)) / sizeof(uint32_t);
        for (int i = 0; i < sdt_num; ++i) {
            process_acpi_table(rsdt->sdt_entries[i]);
        }
    } else {
        // newer than 1.0, treat them as 2.0
        rsdp_2_t *rsdp = (rsdp_2_t *) addr;
        if (!is_rsdp_2_valid(rsdp)) {
            log("RSDP 2.0 is not valid.");
            return false;
        }
        // get XSDT from RSDP
        xsdt_t *xsdt = (xsdt_t *) (rsdp->xsdt_addr);
        // validate XSDT
        if (memcmp(xsdt->header.signature, "XSDT", 4) != 0 || !is_sdt_valid(&(xsdt->header))) {
            log("ACPI::XSDT not valid.");
            return false;
        }
        int sdt_num = (xsdt->header.length - sizeof(xsdt->header)) / sizeof(uint64_t);
        for (int i = 0; i < sdt_num; ++i) {
            process_acpi_table(xsdt->sdt_entries[i]);
        }
    }

    return true;
}