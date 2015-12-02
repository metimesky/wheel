#include "virt_alloc.h"
#include <common/stdhdr.h>
#include <common/util.h>
#include "page_alloc.h"

/* Virtual memory allocator is implemented modeling SLUB allocator.
 * allocator consists of a binlist, each bin contains a list of slabs, each
 * slab contains many packed objects
 */

#define SLAB_SIZE   1       // how many pages in a slab
#define PAGE_SIZE   4096    // how many bytes in a page

/* The kernel virtual memory space map is illustrated as:
 * +-----+--------------+--------------+------------------+----------------
 * | 1MB | Kernel Image | Rest for 4GB | Kernel Heap Meta | Kernel Heap ->
 * +-----+--------------+--------------+------------------+----------------
 * The Wheel kernel set up 4GB identity mapping, so kernel heap are mapped
 * after 4GB.
 * Right after 4GB barrier, there is kernel heap's meta data area, which
 * contains an array of `struct slab`, each `struct slab` correspond to a
 * slab in the kernel heap area located after kernel heap meta area
 */

struct bin {
    uint64_t first_slab;
    uint64_t first_free;
} __attribute__((packed));
typedef struct bin bin_t;

struct slab {
    // slab address is page-aligned, so the
    // lowest 12 bits are free object count
    uint32_t free_num   :12;
    uint64_t next       :52;
} __attribute__((packed));
typedef struct slab slab_t;

// static uint64_t next_slab(uint64_t next_and_free_num) {
//     return next_and_free_num & 0xfffffffffffff000UL;
// }

// each binlist is a double linked list of pages
// where each page contains several objects of
// size 8*i, where i is the index of binlist.
// except the first bin, which is bin for slab-object.

static uint64_t binlist[512];


// allocate 2^order pages and map them to addr, no need to be physically continuous
bool virt_alloc_pages(uint64_t addr, int order) {
    // guarentee that we can allocate such memory.
    if ((free_page_count >> order) == 0) {
        // in this case, we are out of memory.
        return false;
    }
    
    // try allocate all pages once
    uint64_t frame = alloc_pages(order);
    if (frame) {
        // if success, just map them
        int n = 1<<order;   // the number of pages in total
        for (int i = 0; i < n; ++i) {
            map(frame+4096*i, addr+4096*i);
        }
        return true;
    } else {
        // if we can't, split them up and alloc pages recursively
        order -= 1;
        return virt_alloc_pages(addr, order) &&
               virt_alloc_pages(addr+(4096<<order), order);
    }
}

void virt_alloc_init() {
    uint32_t a, d;
    // cpuid()
}

void mem_alloc(int size) {
    // round up to 8 bytes
    int idx = (size + 7) >> 3;
}

void mem_free(uint64_t addr, int size) {
    // calculate the index
    int idx = (size + 7) >> 3;
}

void check_cache_size() {
    // first acquire the cache level and size to align objects in slab
    // so that we could performing cache coloring.
    uint32_t a, b, c, d;
    for (int i = 0; 1; ++i) {
        a = 4;
        c = 0;
        __asm__ __volatile__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(a), "b"(b), "c"(c), "d"(d));
        switch (a & 0x1f) {
        case 0:
            // null, no more caches
            break;
        case 1:
            // data cache
            break;
        case 2:
            // instruction cache
            break;
        case 3:
            // unified cache
            break;
        default:
            // reserved
            break;
        }
        int level = (a >> 5) & 0x07;
    }
}

char buf[33];

block_tag_t *first_block;

void test_mm() {
    // print("asking for order 0 -> 0x");
    // uint64_t addr1 = alloc_pages(0);
    // println(u64_to_str(addr1, buf, 16));

    // map(addr1, 0x100000000UL);
    // char *pointee = (char *) 0x100000000UL;
    // char *msg = "This is the string that is written to the newly allocated memory.";

    // for (int i = 0; msg[i]; ++i) {
    //     pointee[i] = msg[i];
    // }

    // println(pointee);

    // map(addr1, 0x200000000UL);
    // char *g8 = (char *) 0x200000000UL;
    // println(g8);

    // unmap(g8);
    // unmap(pointee);
}

////////////////////////////////////////////////////////////////////////////////
/// Bitmap-based Kernel Heap
////////////////////////////////////////////////////////////////////////////////

// reference: http://wiki.osdev.org/User:Pancakes/SimpleHeapImplementation

// for safety, we map kernel heap right after 4GB, grow upward
uint64_t heap_addr = 0x100000000UL;
uint64_t heap_size;

void kernel_heap_init() {
    // allocate the first page
    uint64_t phy = alloc_pages(0);
    if (phy == 0) {
        log("Cannot init kernel heap, no more memory!");
        return;
    }
    map(phy, heap_addr);
}