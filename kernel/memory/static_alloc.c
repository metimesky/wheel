#include "static_alloc.h"

extern char kernel_bss_end;     // defined in link.lds

// the address of first free byte (the beginning of usable memory).
// useful in `page_alloc`.
uint64_t kernel_end;

void static_alloc_init() {
    kernel_end = (uint64_t) &kernel_bss_end;
}

void *alloc_static(size_t len) {
    // align the address by 16
    kernel_end += (16 - 1);
    kernel_end &= ~(16UL - 1);

    uint64_t ret = kernel_end;
    kernel_end += len;

    return (void *) ret;
}