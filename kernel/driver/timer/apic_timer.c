#include "apic_timer.h"

/* APIC timer frequency is tied to apic bus, so the first task is to measure the actual
 * frequency with the help of PIT or HPET.
 */

// we assume that local apic is already initialized
void apic_timer_init(uint64_t base) {
    ;
}