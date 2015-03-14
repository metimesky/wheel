#include <types.h>

void io_wait() {
    // output to an unused port, 0x80 is used for checkpoints during POST
    asm volatile ("outb %%al, $0x80" :: "a"(0));
}

uint8_t in_byte(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void out_byte(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}
