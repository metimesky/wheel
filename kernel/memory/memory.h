#ifndef __MEMORY_H__
#define __MEMORY_H__

/**
    This is the memory manager of Wheel OS.
    The memory manager is consisted of three layers:
    first layer is physical memory manager (page_alloc.c)
    second layer is virtual memory manager (virt_alloc.c)
    third layer is object cache allocator (slab_alloc.c)
 */

#include <utilities/clib.h>
#include <init/multiboot.h>

#include "page_alloc.h"
#include "slab_alloc.h"

// initialize the memory manager
void memory_init(multiboot_info_t *mbi);

#endif // __MEMORY_H__