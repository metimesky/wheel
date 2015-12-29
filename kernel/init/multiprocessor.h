#ifndef __MULTIPROCESSOR_H__
#define __MULTIPROCESSOR_H__ 1

/**
    Multi-processor information can be found using two methods:
    1. using old MP table, as shown in this file
    2. using new ACPI structure, which is prefered
 */

#include <utilities/clib.h>

#define _MP_ (('_'<<0)|('M'<<8)|('P'<<16)|('_'<<24))

struct mp_pointer {
    uint32_t signature;
    uint32_t pointer;
    uint8_t length;
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t feature[5];
} __attribute__((packed));
typedef struct mp_pointer mp_pointer_t;

#define PCMP (('P'<<0)|('C'<<8)|('M'<<16)|('P'<<24))

struct mp_conf {
    uint32_t signature;
    uint16_t base_length;
    uint8_t spec_rev;
    uint8_t checksum;
    char oem_id[8];
    char product_id[12];
    uint32_t oem_table_pointer;
    uint16_t oem_table_size;
    uint16_t entry_count;
    uint32_t local_apic_addr;
    uint16_t ext_length;
    uint8_t ext_checksum;
    uint8_t reserved;
    uint8_t entries[0];
} __attribute__((packed));
typedef struct mp_conf mp_conf_t;

struct mp_processor_entry {
    uint8_t type;   // 0
    uint8_t local_apic_id;
    uint8_t local_apic_version;
    uint8_t cpu_flags;
    uint32_t cpu_signature;
    uint32_t feature_flags;
    uint32_t reserved_1;
    uint32_t reserved_2;
} __attribute__((packed));
typedef struct mp_processor_entry mp_processor_entry_t;

struct mp_bus_entry {
    uint8_t type;   // 1
    uint8_t bus_id;
    char bus_type[6];
} __attribute__((packed));
typedef struct mp_bus_entry mp_bus_entry_t;

struct mp_io_apic_entry {
    uint8_t type;   // 2
    uint8_t io_apic_id;
    uint8_t io_apic_version;
    uint8_t io_apic_flags;
    uint32_t address;
} __attribute__((packed));
typedef struct mp_io_apic_entry mp_io_apic_entry_t;

struct mp_io_interrupt_entry {
    uint8_t type;   // 3
    uint8_t interrupt_type;
    uint16_t io_interrupt_flags;
    uint8_t src_bus_id;
    uint8_t src_bus_irq;
    uint8_t dst_io_apic_id;
    uint8_t dst_io_apic_intin;
} __attribute__((packed));
typedef struct mp_io_interrupt_entry mp_io_interrupt_entry_t;

struct mp_local_interrupt_entry {
    uint8_t type;   // 4
    uint8_t interrupt_type;
    uint16_t local_interrupt_flags;
    uint8_t src_bus_id;
    uint8_t src_bus_irq;
    uint8_t dst_local_apic_id;
    uint8_t dst_local_apic_lintin;
} __attribute__((packed));
typedef struct mp_local_interrupt_entry mp_local_interrupt_entry_t;

#endif // __MULTIPROCESSOR_H__