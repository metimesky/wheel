#ifndef __MULTIPROCESSOR_H__
#define __MULTIPROCESSOR_H__ 1

#include <type.h>

#define _MP_ (('_'<<0)|('M'<<8)|('P'<<16)|('_'<<24))

struct mp_pointer {
    uint32_t signature;
    uint32_t pointer;
    uint8_t length;
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t feature[5];
};
typedef struct mp_pointer mp_pointer_t;

#define PCMP (('P'<<0)|('C'<<0)|('M'<<0)|('P'<<0))

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
};
typedef struct mp_conf mp_conf_t;

#endif // __MULTIPROCESSOR_H__