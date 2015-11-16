#ifndef __MEMORY_H__
#define __MEMORY_H__

/**
    This is the memory manager of Wheel OS.
    The memory manager is consisted of three layers:
    first layer is physical memory manager (page_alloc.c)
    second layer is virtual memory manager (virt_alloc.c)
    third layer is object cache allocator (slab_alloc.c)
 */

#include <common/stdhdr.h>
#include <common/multiboot.h>

/**
    initialize memory manager
 */
void memory_init(multiboot_info_t *mbi);

////////////////////////////////////////////////////////////////////////////////
/// Page Allocator
////////////////////////////////////////////////////////////////////////////////

/**
    initialize page allocator
 */
void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

/** (private)
    find free 2^order continuous pages and return the starting physical address
 */
uint64_t find_free_pages(int order);

/**
    allocate 2^order continuous pages and return the starting physical address
 */
uint64_t alloc_pages(int order);

/**
    release 2^order continuous pages at starting address `addr`
 */
void free_pages(uint64_t addr, int order);

/**
    map physical address `frame` to kernel virtual address `page`
    frame and page must be page aligned
 */
bool map(uint64_t frame, uint64_t page);

/**
    clear the paging entry of virtual address `page`.
 */
void unmap(uint64_t page);

#endif // __MEMORY_H__