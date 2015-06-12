#include <env.h>
#include "page_alloc.h"
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

uint8_t* buddy_map[8];
uint64_t buddy_num[8];

// param: size -- the size of physical memory in KB.
void paging_init(uint32_t mmap_addr, uint32_t mmap_length) {
    uint64_t i = 0;
    // initialize 4K buddy map
    buddy_map[0] = (uint8_t *) &kernel_bss_end;
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;
    while ((uint32_t) mmap < (mmap_addr + mmap_length)) {
        if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
            uint64_t start = mmap->addr;
            uint64_t end   = start + mmap->len;

            // round start upward and end downward to 4K boundary
            start += 0x1000 - 1;
            start >>= 12;
            end >>= 12;

            // fill the gap between two usable memory region
            for (; i < start; ++i) {
                buddy_map[0][i >> 3] |= 1 << (i & 7);
            }

            // fill the range (this implicitly rounded down `end` to 4K)
            for (; i < end; ++i) {
                buddy_map[0][i >> 3] &= ~(1 << (i & 7));
            }
        }
        mmap = (multiboot_memory_map_t *)((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }
    frame_num = i;
    buddy_num[0] = (i + 127) & ~127;
    // fill the rest part with ones
    for (; i < buddy_num[0]; ++i) {
        buddy_map[0][i >> 3] |= 1 << (i & 7);
    }

    // set buddy level 2 to 8
    for (int level = 1; level < 8; ++level) {
        buddy_num[level] = buddy_num[level-1] >> 1;
        buddy_map[level] = buddy_map[level-1] + ((buddy_num[level-1] + 7) >> 3);
        for (i = 0; i < buddy_num[level]; ++i) {
            if (buddy_map[level-1][i>>2] & (i&3) || buddy_map[level-1][i>>2] & (i&3 + 1)) {
                buddy_map[level][i >> 3] |= i << (i & 7);
            } else {
                buddy_map[level][1 >> 3] &= ~(1 << (i & 7));
            }
        }
    }
    // // frames start from the end of kernel, align upward to 4KB.
    // frames = (frame_t *) (((uint64_t) &kernel_bss_end + 0x1000 - 1) & ~(0x1000 - 1));

    // multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;
    // while ((uint32_t) mmap < (mmap_addr + mmap_length)) {
    //     if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
    //         uint64_t start = mmap->addr;
    //         uint64_t end   = start + mmap->len;

    //         // if start address is not 4K aligned, round upward
    //         if (start % 0x1000 != 0) {
    //             start += 0x1000;
    //             start &= ~(0x1000 - 1);
    //         }

    //         // fill the gap between two usable memory region
    //         for (; frame_num < (start >> 12); ++frame_num) {
    //             // mark this frame as unusable
    //             frames[frame_num].attr = 0;
    //         }

    //         // fill the range (this implicitly rounded down `end` to 4K)
    //         for (; frame_num < (end >> 12); ++frame_num) {
    //             // mark this frame as usable
    //             frames[frame_num].attr = 1;
    //         }
    //     }
    //     mmap = (multiboot_memory_map_t *)((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    // }

    // return frame_num;
}