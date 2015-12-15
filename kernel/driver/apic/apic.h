#ifndef __APIC_H__
#define __APIC_H__ 1

#include <common/stdhdr.h>

void io_apic_init(uint64_t base);
void local_apic_init(uint64_t base);

#endif // __APIC_H__