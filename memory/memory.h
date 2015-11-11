#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdhdr.h>
#include <multiboot.h>

void memory_init(multiboot_info_t *mbi);

#endif // __MEMORY_H__