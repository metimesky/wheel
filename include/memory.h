#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <wheel.h>

extern uint64_t kernel_end_addr;
extern uint64_t __percpu_offset;

extern void percpu_area_init();
extern void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

extern uint64_t alloc_pages(int order);
extern void free_pages(uint64_t addr, int order);

// extern bool map(uint64_t phy, uint64_t virt);

extern void reclaim_init_space();

#endif