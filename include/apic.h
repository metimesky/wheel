#ifndef __APIC_H__
#define __APIC_H__

#include <drivers/acpi/acpi.h>

// parse MADT
extern void apic_preinit();

// extern void io_apic_preinit();
extern void io_apic_add(ACPI_MADT_IO_APIC *io_apic);
extern void io_apic_interrupt_override(ACPI_MADT_INTERRUPT_OVERRIDE *override);
extern void io_apic_init();
extern void io_apic_mask(int gsi);
extern void io_apic_unmask(int gsi);

// extern void local_apic_preinit(uint64_t address);
extern void local_apic_add(ACPI_MADT_LOCAL_APIC *local_apic);
extern void local_apic_address_override(ACPI_MADT_LOCAL_APIC_OVERRIDE *override);
extern void local_apic_init();

#endif