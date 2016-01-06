#include "memory.h"
#include <interrupt/interrupt.h>

// defined in page_alloc.c
extern void page_fault_handler(int vec, interrupt_context_t *ctx);

void memory_init(multiboot_info_t *mbi) {
    // init page allocator (creating buddy)
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);

    // setup page fault handler
    interrupt_install_handler(14, page_fault_handler);

    // init virt allocator (creating binlists)
    slab_alloc_init();
}