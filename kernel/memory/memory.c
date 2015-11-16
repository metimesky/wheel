#include "memory.h"
#include "page_alloc.h"

void memory_init(multiboot_info_t *mbi) {
    // init page allocator (creating buddy)
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);

    // init virt allocator (creating binlists)
    // virt_alloc_init();
}