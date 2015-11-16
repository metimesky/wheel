#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__ 1

/*
    page_alloc only cares about page level, like page allocation, deallocation,
    map and unmap.
*/

#include <common/stdhdr.h>

void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

uint64_t find_free_pages(int order);
uint64_t alloc_pages(int order);
void free_pages(uint64_t addr, int order);

bool map(uint64_t frame, uint64_t page);
void unmap(uint64_t page);

#endif // __PAGE_ALLOC_H__
