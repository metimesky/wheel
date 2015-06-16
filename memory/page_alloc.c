#include "page_alloc.h"
#include <env.h>
#include <multiboot.h>
#include <bitmap.h>

extern char kernel_bss_end;

uint64_t *buddy_map[8];     // in the bitmap, 1 indicates free and usable page
uint64_t buddy_num[8];      // how many blocks are there for each order

// shrink data into half by and neighbouring bits
// only higher half of the return value is valid
static uint64_t bit_merge_up(uint64_t data) {
    uint64_t ret = data;
    ret &= ret << 1;
    ret &= 0xaaaaaaaaaaaaaaaaUL;
    // X0X0X0X0|X0X0X0X0|X0X0X0X0|X0X0X0X0|X0X0X0X0|X0X0X0X0|X0X0X0X0|X0X0X0X0

    ret |= (ret << 1) & 0x5555555555555555UL;
    ret &= 0xccccccccccccccccUL;
    // XX00XX00|XX00XX00|XX00XX00|XX00XX00|XX00XX00|XX00XX00|XX00XX00|XX00XX00

    ret |= (ret << 2) & 0x3333333333333333UL;
    ret &= 0xf0f0f0f0f0f0f0f0UL;
    // XXXX0000|XXXX0000|XXXX0000|XXXX0000|XXXX0000|XXXX0000|XXXX0000|XXXX0000

    ret |= (ret << 4) & 0x0f0f0f0f0f0f0f0fUL;
    ret &= 0xff00ff00ff00ff00UL;
    // XXXXXXXX|00000000|XXXXXXXX|00000000|XXXXXXXX|00000000|XXXXXXXX|00000000

    ret |= (ret << 8) & 0x00ff00ff00ff00ffUL;
    ret &= 0xffff0000ffff0000UL;
    // XXXXXXXX|XXXXXXXX|00000000|00000000|XXXXXXXX|XXXXXXXX|00000000|00000000

    ret |= (ret << 16) 0x0000ffff0000ffffUL;
    ret &= 0xffffffff00000000UL;
    // XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX|00000000|00000000|00000000|00000000

    return ret;
}

// shrink data into half by and neighbouring bits
// only lower half of the return value is valid
static uint64_t bit_merge_down(uint64_t data) {
    uint64_t ret = data;
    ret &= ret >> 1;
    ret &= 0x5555555555555555UL;
    // 0X0X0X0X|0X0X0X0X|0X0X0X0X|0X0X0X0X|0X0X0X0X|0X0X0X0X|0X0X0X0X|0X0X0X0X

    ret |= (ret >> 1) & 0xaaaaaaaaaaaaaaaaUL;
    ret &= 0x3333333333333333UL;
    // 00XX00XX|00XX00XX|00XX00XX|00XX00XX|00XX00XX|00XX00XX|00XX00XX|00XX00XX

    ret |= (ret >> 2) & 0xccccccccccccccccUL;
    ret &= 0x0f0f0f0f0f0f0f0fUL;
    // 0000XXXX|0000XXXX|0000XXXX|0000XXXX|0000XXXX|0000XXXX|0000XXXX|0000XXXX

    ret |= (ret >> 4) & 0xf0f0f0f0f0f0f0f0UL;
    ret &= 0x00ff00ff00ff00ffUL;
    // 00000000|XXXXXXXX|00000000|XXXXXXXX|00000000|XXXXXXXX|00000000|XXXXXXXX

    ret |= (ret >> 8) & 0xff00ff00ff00ff00UL;
    ret &= 0x0000ffff0000ffffUL;
    // 00000000|00000000|XXXXXXXX|XXXXXXXX|00000000|00000000|XXXXXXXX|XXXXXXXX

    ret |= (ret >> 16) 0xffff0000ffff0000UL;
    ret &= 0x00000000ffffffffUL;
    // 00000000|00000000|00000000|00000000|XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX

    return ret;
}

void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // initialize 4K buddy map using multiboot info
    buddy_map[0] = (uint64_t *) (((uint64_t) &kernel_bss_end + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1));
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;
    int start = -1, end = -1;
    while ((uint32_t) mmap < (mmap_addr + mmap_length)) {
        if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
            start = mmap->addr;
            end = start + mmap->len;

            // round start upward and end downward to 4K boundary
            start += 4096 - 1;
            start >>= 12;
            end >>= 12;

            // fill the gap between two usable memory region
            bitmap_clear(buddy_map[0], frame_num, start - frame_num);

            // fill the range
            bitmap_set(buddy_map[0], start, end - start);
        }
        mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }

    // set extra bits to zero
    buddy_map[end / BITS_PER_UINT64] &= BITMAP_LAST_UINT64_MASK(end);

    // total frame number of 4K buddy
    buddy_num[0] = end;
    
    // set buddy order 2 to 8
    for (int order = 1; order < 8; ++order) {
        // Note it's the number of previous order map
        int num_of_uint64 = BITS_TO_UINT64(buddy_num[order - 1]);

        // how many blocks we have for this order
        buddy_map[order] = buddy_map[order - 1] + num_of_uint64;
        buddy_num[order] = (buddy_num[order - 1] + (1 << order) - 1) >> order;

        bitmap_zero(buddy_map[order], buddy_num[order]);

        // Note that we are iterating over PREVIOUS order of map!
        for (int i = 0; i < num_of_uint64; ++i) {
            buddy_map[order][i >> 1] |= (i & 1)
                                      ? bit_merge_up(buddy_map[order - 1][i])
                                      : bit_merge_down(buddy_map[order - 1][i]);
        }
    }
}