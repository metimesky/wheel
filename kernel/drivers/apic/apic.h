#ifndef __APIC_H__
#define __APIC_H__ 1

#include <utilities/env.h>

#include "local_apic.h"
#include "io_apic.h"

////////////////////////////////////////////////////////////////////////////////
/// Initialization
////////////////////////////////////////////////////////////////////////////////

extern bool apic_init();
extern bool apic_mp_init();

#endif // __APIC_H__