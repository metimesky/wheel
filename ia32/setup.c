#include <types.h>
#include "protect.h"

extern void setup_gdt();
extern void setup_interrupt();

void setup(uint32_t magic, uint32_t mbi) {
    setup_gdt();
    setup_interrupt();
}
