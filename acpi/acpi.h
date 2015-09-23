#ifndef __ACPI_H__
#define __ACPI_H__ 1

/* This file deals with Root System Description Table */

#include <type.h>

struct rsdp_1 {
    char signature[8];  // "RSD PTR "
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;
} __attribute__((packed));
typedef struct rsdp_1 rsdp_1_t;

struct rsdp_2 {
    rsdp_1_t rsdp_1;
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __attribute__((packed));
typedef struct rsdp_2 rsdp_2_t;

// the header is shared in RSDT and XSDT
// the standard ACPI table header
struct sdt_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));
typedef struct sdt_header sdt_header_t;

struct rsdt {   // signature = "RSDT"
    sdt_header_t header;
    uint32_t sdt_addrs[0];
} __attribute__((packed));

struct xsdt {   // signature = "XSDT"
    sdt_header_t header;
    uint64_t sdt_addrs[0];
} __attribute__((packed));

// MADT table
struct madt {   // signature = "MADT"
    sdt_header_t header;
    uint32_t local_controller_addr;
    uint32_t flags;
    uint8_t* entries[0];
} __attribute__((packed));
typedef struct madt madt_t;

// entries that may following MADT

struct madt_local_apic_entry {
    uint8_t type;   // 0
    uint8_t length;
    uint8_t processor_id;
    uint8_t local_apic_id;
    uint32_t flags;
} __attribute__((packed));
typedef struct madt_local_apic_entry madt_local_apic_entry_t;

struct madt_io_apic_entry {
    uint8_t type;   // 1
    uint8_t length;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed));
typedef struct madt_io_apic_entry madt_io_apic_entry_t;

struct madt_interrupt_src_override {
    uint8_t type;
    uint8_t length;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed));
typedef struct madt_interrupt_src_override madt_interrupt_src_override_t;

#endif // __ACPI_H__