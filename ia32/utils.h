#ifndef IA32_UTILS_H_
#define IA32_UTILS_H_ 1

#include <types.h>
#include "protect.h"

void load_gdtr(gdt_ptr_t*);
void load_idtr(idt_ptr_t*);
void load_tr(uint16_t sel);

uint8_t in_byte(uint16_t port);
void out_byte(uint16_t port, uint8_t data);

#endif
