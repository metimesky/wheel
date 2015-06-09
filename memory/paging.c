#include <env.h>
#include "paging.h"
#include <multiboot.h>

extern char kernel_bss_end;

//typedef unsigned long long frame_t;

#define USED_4K(f)  ((f) &  1)
#define USED_2M(f)  ((f) &  2)
#define USED_1G(f)  ((f) &  4)
#define PAGE_4K(f)  ((f) &  8)
#define PAGE_2M(f)  ((f) & 16)
#define PAGE_1G(f)  ((f) & 32)
#define VIRTUAL(f)  ((f) & 0xfffffffffffff000)

frame_t *frames = NULL;
uint64_t frame_num = 0;     // how many 4K frames are there

// param: size -- the size of physical memory in KB.
uint64_t paging_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // frames start from the end of kernel, align upward to 4KB.
    frames = (frame_t *) (((uint64_t) &kernel_bss_end + 0x1000 - 1) & ~(0x1000 - 1));

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;
    while ((uint32_t) mmap < (mmap_addr + mmap_length)) {
        if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
            uint64_t start = mmap->addr;
            uint64_t end   = start + mmap->len;

            // if start address is not 4K aligned, round upward
            if (start % 0x1000 != 0) {
                start += 0x1000;
                start &= ~(0x1000 - 1);
            }

            // fill the gap between two usable memory region
            for (; frame_num < (start >> 12); ++frame_num) {
                // mark this frame as unusable
                frames[frame_num].attr = 0;
            }

            // fill the range (this implicitly rounded down `end` to 4K)
            for (; frame_num < (end >> 12); ++frame_num) {
                // mark this frame as usable
                frames[frame_num].attr = 1;
            }
        }
        mmap = (multiboot_memory_map_t *)((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }

    return frame_num;
}