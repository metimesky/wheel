#ifndef __ACPI_H__
#define __ACPI_H__ 1

/* This file deals with Root System Description Table */

#include <stdhdr.h>

/*******************************************************************************
 * Pointer and General Structures
 ******************************************************************************/

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
    uint32_t sdt_entries[0];
} __attribute__((packed));
typedef struct rsdt rsdt_t;

struct xsdt {   // signature = "XSDT"
    sdt_header_t header;
    uint64_t sdt_entries[0];
} __attribute__((packed));
typedef struct xsdt xsdt_t;

#endif // __ACPI_H__