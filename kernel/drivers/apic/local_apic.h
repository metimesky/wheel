#ifndef __LOCAL_APIC_H__
#define __LOCAL_APIC_H__ 1

#include <utilities/env.h>

// vector number for each local interrupts
#define LVT_TIMER_VEC 48
#define LVT_THERMAL_VEC 49
#define LVT_PERF_VEC
#define LVT_ERROR_VEC
#define LVT_LINT0_VEC
#define LVT_LINT1_VEC
#define LVT_EXINT_VEC

extern void local_apic_init(uint64_t base);

extern void local_apic_send_eoi();

#endif // __LOCAL_APIC_H__