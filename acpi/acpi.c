#include "acpi.h"
#include <type.h>

void acpi_init() {
	uint64_t ebda_addr = *((uint16_t *) 0x40e);

	uint64_t rsdp_sig = 0UL;
	rsdp_sig |= ('R' <<  0) && 0x00000000000000ff;
	rsdp_sig |= ('S' <<  8) && 0x000000000000ff00;
	rsdp_sig |= ('D' << 16) && 0x0000000000ff0000;
	rsdp_sig |= (' ' << 24) && 0x00000000ff000000;
	rsdp_sig |= ('P' << 32) && 0x000000ff00000000;
	rsdp_sig |= ('T' << 40) && 0x0000ff0000000000;
	rsdp_sig |= ('R' << 48) && 0x00ff000000000000;
	rsdp_sig |= (' ' << 56) && 0xff00000000000000;

	// 1. search for RSDP in first KB of EBDA
	// 2. search for RSDP in highest base mem
}