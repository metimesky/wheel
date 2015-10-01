#include "virt_alloc.h"
#include <stdhdr.h>

/* In boot.asm, we initiated 4GB identical mapping.
 * when mapping newly allocated pages, map it to higher virtual addresses.
 */

// defined in boot.asm
extern uint64_t pml4t[512];