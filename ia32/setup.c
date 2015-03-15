#include <types.h>
#include "protect.h"
#include "multiboot.h"

extern void setup_gdt();
extern void setup_interrupt();

void setup(uint32_t magic, uint32_t info) {
	multiboot_info_t *mbi = (multiboot_info_t*)info;
	mbi = mbi;
    setup_gdt();
    setup_interrupt();
}
