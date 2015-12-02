#ifndef __VIRT_ALLOC_H__
#define __VIRT_ALLOC_H__ 1

#include <common/stdhdr.h>

struct block_tag {
    uint64_t prev_block;
    uint64_t block_size;
} __attribute__((packed));
typedef struct block_tag block_tag_t;

struct mem_chunk {
    struct mem_chunk *prev;
    struct mem_chunk *next;
    size_t size;
} __attribute__((packed));
typedef struct mem_chunk mem_chunk_t;

// allocate 2^order physical pages and map them continuously to vertual address addr
bool virt_alloc_pages(uint64_t addr, int order);

#endif // __VIRT_ALLOC_H__