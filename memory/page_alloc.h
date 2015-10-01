#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__ 1

#include <stdhdr.h>

void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

uint64_t find_free_pages(int order);
uint64_t alloc_pages(int order);
void free_pages(uint64_t addr, int order);

#endif // __PAGE_ALLOC_H__
