#ifndef __HPET_H__
#define __HPET_H__ 1

#include <driver/acpi/acpi.h>

struct addr_struct {
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed));
typedef struct addr_struct addr_struct_t;

struct hpet {
    sdt_header_t header;
    uint8_t hardware_rev_id;
    uint8_t comparator_count:5;
    uint8_t counter_size:1;
    uint8_t reserved:1;
    uint8_t legacy_replacement:1;
    uint16_t pci_vendor_id;
    addr_struct_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__((packed));
typedef struct hpet hpet_t;

extern void process_hpet(hpet_t *hpet);

#endif // __HPET_H__