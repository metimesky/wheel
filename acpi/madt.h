#ifndef __MADT_H__
#define __MADT_H__ 1

#include <stdhdr.h>
#include "acpi.h"

/*******************************************************************************
 * MADT - Multiple APIC Description Table
 ******************************************************************************/

struct madt {   // signature = "APIC"
    sdt_header_t header;
    uint32_t local_controller_addr;
    uint32_t flags;
    uint8_t entries[0];
} __attribute__((packed));
typedef struct madt madt_t;

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
    uint8_t type;   // 2
    uint8_t length;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed));
typedef struct madt_interrupt_src_override madt_interrupt_src_override_t;

struct madt_nmi_source {
    uint8_t type;   // 3
    uint8_t length;
} __attribute__((packed));
typedef struct madt_nmi_source madt_nmi_source_t;

struct madt_local_apic_nmi {
    uint8_t type;   // 4
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_apic_nmi madt_local_apic_nmi_t;

struct madt_local_apic_addr_override {
    uint8_t type;   // 5
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_apic_addr_override madt_local_apic_addr_override_t;

struct madt_io_sapic {
    uint8_t type;   // 6
    uint8_t length;
} __attribute__((packed));
typedef struct madt_io_sapic madt_io_sapic_t;

struct madt_local_sapic {
    uint8_t type;   // 7
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_sapic madt_local_sapic_t;

struct madt_platform_interrupt_source {
    uint8_t type;   // 8
    uint8_t length;
} __attribute__((packed));
typedef struct madt_platform_interrupt_source madt_platform_interrupt_source_t;

#endif // __MADT_H__