#ifndef __VIRT_ALLOC_H__
#define __VIRT_ALLOC_H__ 1

#include <stdhdr.h>

struct pml4e {
    uint32_t p:1;
    uint32_t rw:1;
    uint32_t us:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t a:1;
    uint32_t ign:1;
    uint32_t mbz:2;
    uint32_t avl:3;
    uint64_t pdp_base:40;
} __attribute__((packed));
typedef struct pml4e pml4e_t;

#endif // __VIRT_ALLOC_H__