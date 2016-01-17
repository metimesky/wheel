#ifndef __IO_APIC_H__
#define __IO_APIC_H__ 1

#include <utilities/env.h>
#include <drivers/acpi/acpi.h>

// #define APIC_GSI_VEC_BASE 80

/* Initialization order:
 * first, call `io_apic_init`.
 * then, for each IO APIC, call `io_apic_add`, 
 * also, for each interrupt override, call `io_apic_interrupt_override`.
 */

extern void io_apic_init();
extern void io_apic_add(ACPI_MADT_IO_APIC *ioapic);
extern void io_apic_interrupt_override(ACPI_MADT_INTERRUPT_OVERRIDE *override);

extern void io_apic_set_gsi(uint32_t gsi, uint64_t ent);
extern void io_apic_mask(uint32_t gsi);
extern void io_apic_unmask(uint32_t gsi);

#endif // __IO_APIC_H__