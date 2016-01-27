#ifndef __STATIC_ALLOC_H__
#define __STATIC_ALLOC_H__ 1

#include <utilities/env.h>

/* static allocator is a special memory allocator, which allocates memory
 * and never reclaims it.
 * It can only be used before page allocator and slab allocator.
 * The address it returns is physical address.
 */

/* Static allocator works by increasing kernel_end position. Once dynamic
 * memory is initialized (i.e. page_alloc), this method won't work!
 */

void static_alloc_init();
uint64_t alloc_static(size_t len);

#endif // __STATIC_ALLOC_H__