#ifndef __IO_APIC_H__
#define __IO_APIC_H__ 1

#include <utilities/env.h>
#include <drivers/acpi/acpi.h>

#define APIC_GSI_VEC_BASE 80

extern void io_apic_init(ACPI_MADT_IO_APIC *ioapic, ACPI_MADT_INTERRUPT_OVERRIDE *override[], int n);

#endif // __IO_APIC_H__