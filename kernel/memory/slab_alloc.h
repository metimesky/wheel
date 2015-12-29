#ifndef __VIRT_ALLOC_H__
#define __VIRT_ALLOC_H__ 1

#include <utilities/clib.h>

// initialize slab allocator
void slab_alloc_init();

// allocate an object
void* slab_alloc(int size);

// free an allocated object
void slab_free(void *addr);

#endif // __VIRT_ALLOC_H__