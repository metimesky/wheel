#ifndef __VIRT_ALLOC_H__
#define __VIRT_ALLOC_H__ 1

#include <utilities/env.h>

// initialize slab allocator
void slab_alloc_init();

// allocate an object
void* slab_alloc(size_t size);

// free an allocated object
void slab_free(void *addr);

#endif // __VIRT_ALLOC_H__