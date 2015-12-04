#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__ 1

/* This is the physical memory manager of Wheel OS
 * allocate and deallocate pages, implemented using buddy algorithm
 *******************************************************************************
 * An important property of this page allocator is that, you can ask for many
 * pages at once and free them individually, allocation and free do not need to
 * be symmetric.
 */

#include <common/stdhdr.h>

// initialize page allocator, passing mmap from multiboot_info as argument
void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

// READONLY! the number of unallocated pages
extern uint64_t free_page_count;

/*******************************************************************************
 * the following two functions do not need to be called in pairs
 */

// find and allocate 2^order continuous free pages, return the physical address
// of the first page, or 0 if could not find one.
// the pages are aligned at 2^order pages' size.
uint64_t alloc_pages(int order);

// reclaim 2^order continuous pages at physical address `addr`
// the pages must be 2^order-page-aligned, so that it could fit in certain buddy
void free_pages(uint64_t addr, int order);

/*******************************************************************************
 */

// map the virtual page to physical frame.
bool map(uint64_t page, uint64_t frame);

// clear the mapping of virtual page
void unmap(uint64_t page);

// get the physical address mapped by virtual address
uint64_t virt_to_phy(uint64_t addr);

/*******************************************************************************
 * the following two functions do not need to be called in pairs
 */

// allocate pages on virtual address space
bool virt_alloc_pages(uint64_t addr, int order);

// free pages allocated on virtual address space
void virt_free_pages(uint64_t addr, int order);

#endif // __PAGE_ALLOC_H__
