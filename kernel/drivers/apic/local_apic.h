#ifndef __LOCAL_APIC_H__
#define __LOCAL_APIC_H__ 1

#include <utilities/env.h>
#include <drivers/acpi/acpi.h>

// vector number for each local interrupts
#define LVT_TIMER_VEC 48
#define LVT_THERMAL_VEC 49
#define LVT_PERF_VEC
#define LVT_ERROR_VEC
#define LVT_LINT0_VEC
#define LVT_LINT1_VEC
#define LVT_EXINT_VEC


// at most 256 local APICs are supported
#define MAX_LOCAL_APIC_NUM 256

// other modules need to access these fields
extern uint64_t local_apic_base;
extern int local_apic_count;
extern ACPI_MADT_LOCAL_APIC* local_apic_arr[MAX_LOCAL_APIC_NUM];

/* Initialization order:
 * first, call `local_apic_init`.
 * then, for each local apic entry, call `local_apic_add`,
 * also, for each local apic override entry, call `local_apic_address_override`.
 */

extern void local_apic_init(ACPI_TABLE_MADT *madt);
extern void local_apic_add(ACPI_MADT_LOCAL_APIC *lapic);
extern void local_apic_address_override(ACPI_MADT_LOCAL_APIC_OVERRIDE *override);
extern void local_apic_local_init();

extern uint8_t local_apic_get_id(int index);
extern uint8_t local_apic_get_processor_id(int index);
extern void local_apic_send_ipi(uint64_t icr);
extern void local_apic_send_eoi();

#endif // __LOCAL_APIC_H__