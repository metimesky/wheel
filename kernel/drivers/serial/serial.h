#ifndef __SERIAL_H__
#define __SERIAL_H__ 1

// the following port address is not reliable
// we should get the actual address from BIOS Data Area
#define COM1 0x03f8
#define COM2 0x02f8
#define COM3 0x03e8
#define COM4 0x02e8

void serial_init();

#endif // __SERIAL_H__