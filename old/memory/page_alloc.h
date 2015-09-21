#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__ 1

#include <env.h>

/*
struct frame {
    uint64_t linear;    // the linear address this frame mapped to
    uint32_t pid;       // the process that is owning this frame
    uint32_t attr;      // present, granularity
} __attribute__((packed));
typedef struct frame frame_t;
*/

void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length);

uint64_t find_free_pages(int order);
uint64_t alloc_pages(int order);
void free_pages(uint64_t addr, int order);

#endif // __PAGE_ALLOC_H__