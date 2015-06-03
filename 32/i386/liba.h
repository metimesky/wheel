#ifndef LIBA_H_
#define LIBA_H_ 1

uint8_t in_byte(uint16_t port);
void out_byte(uint16_t port, uint8_t data);
uint16_t in_word(uint16_t port);
void out_word(uint16_t port, uint16_t data);
uint32_t in_dword(uint16_t port);
void out_dword(uint16_t port, uint32_t data);
void io_wait();

void load_gdtr(gdt_ptr_t*);
void load_idtr(idt_ptr_t*);
void load_tr(uint16_t sel);

#endif // LIBA_H_
