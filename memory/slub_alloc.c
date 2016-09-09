// #include <memory.h>

// // SLUB应该考虑到多核的情况

// /* Virtual memory allocator is implemented modeling SLUB allocator.
//  * allocator consists of a binlist, each bin contains a list of slabs, each
//  * slab contains many packed objects
//  */

// #define SLAB_SIZE   1       // how many pages in a slab
// #define PAGE_SIZE   4096    // how many bytes in a page

// /* The kernel virtual memory space map is illustrated as:
//  * +-----+--------------+-----------------+------------------+----------------
//  * | 1MB | Kernel Image | padding for 4GB | Kernel Heap Meta | Kernel Heap ->
//  * +-----+--------------+-----------------+------------------+----------------
//  * The Wheel kernel set up 4GB identity mapping, so kernel heap are mapped
//  * after 4GB.
//  * Right after 4GB barrier, there is kernel heap's meta data area, which
//  * contains an array of `struct slab`, each `struct slab` correspond to a
//  * slab in the kernel heap area located after kernel heap meta area
//  */

// extern uint64_t free_page_count;
// uint64_t kernel_heap_start; // the starting address of kernel heap, page aligned
// uint64_t kernel_heap_size;  // how many pages are there in kernel heap (i.e. num of slab)

// /* With above two variable, we can calculate the number of pages in kernel heap:
//  * NumPages = (kernel_end - kernel_start) >> 12;
//  */

// struct slab {
//     uint64_t next;          // the address of next slab page, not address of struct
//     void *free_list;        // the address of first free object within THIS slab
// } __attribute__((packed));
// typedef struct slab slab_t;

// slab_t *kernel_heap_meta;   // the address of kernel heap meta, an array of `slab_t`
// // uint64_t kernel_heap_meta_size; // how many PAGES are there in kernel heap meta

// uint64_t bins[512];         // 512 bins for each size of slabs, item is the virtual
//                             // address of actual page in hernel heap area

// void slab_alloc_init() {
//     // kernel heap meta area starts right at 4GB mark
//     kernel_heap_meta = (slab_t *) 0x100000000UL;

//     // calculate kheap meta size using total free page count (rounding up)
//     kernel_heap_start = kernel_heap_meta;
//     kernel_heap_start += free_page_count * sizeof(slab_t *);
//     kernel_heap_start += 4096 - 1;
//     kernel_heap_start &= ~(4096UL - 1);

//     // at first, kernel heap is empty
//     kernel_heap_size = 0;

//     // at first, every bin is empty
//     for (int i = 0; i < 512; ++i) {
//         bins[i] = NULL;
//     }
// }

// // get the corresponding meta index of a page in kernel heap
// static int index_of_page(uint64_t addr) {
//     return (addr - kernel_heap_start) >> 12;
// }

// // create a new slab with given size, return the virtual address
// // only create slab page, nothing to do with heap meta
// static uint64_t create_slab(int index) {
//     // allocate a new page for the slab
//     uint64_t slab_phy = alloc_pages(0);

//     // the virtual address of the new slab (end of current kernel heap)
//     uint64_t slab_vir = kernel_heap_start + kernel_heap_size * 4096;

//     // setup page entry and increase kernel heap size
//     map(kernel_heap_start + kernel_heap_size * 4096, slab_phy);
//     ++kernel_heap_size;

//     // create free list.
//     int obj_num = 4096 / (8 * index);   // total number of objects in a slab
//     uint64_t *pos = (uint64_t *) slab_vir;
//     for (int i = 0; i < obj_num; ++i) {
//         pos[index * i] = (uint64_t) &pos[index * (i + 1)];
//     }
//     // last one is special, since it's the last element of the list
//     pos[index * (obj_num - 1)] = 0;

//     return slab_vir;
// }

// // create a bin at `index`, the corresponding object size is 8*index
// static void create_bin(int index) {
//     // calculate the kheap meta size in bytes
//     uint64_t kheap_meta_size = kernel_heap_size * sizeof(slab_t *);

//     // check if the kheap meta is about to grow a new page.
//     if (kheap_meta_size % 4096 == 0) {
//         // allocate new physical page for kheap meta
//         uint64_t phy = alloc_pages(0);

//         // calculate the virtual address of the new page
//         uint64_t vir = (uint64_t) kernel_heap_meta + (kheap_meta_size & ~(4096UL - 1));

//         map(vir, phy);
//     }

//     // create new slab and calculate its index in meta
//     uint64_t slab_vir = create_slab(index);
//     int slab_index = index_of_page(slab_vir);

//     // fill slab struct in meta area
//     kernel_heap_meta[slab_index].next = 0;
//     kernel_heap_meta[slab_index].free_list = slab_vir;

//     // point the bin to the page address
//     bins[index] = slab_vir;
// }

// // allocate an object with given size, return the virtual address
// void* slab_alloc(size_t size) {
//     // round up to 8 bytes and calculate the bin index
//     int index = (size + 7) >> 3;

//     if (bins[index] == NULL) {
//         // if this bin is empty, create it first
//         create_bin(index);
//     }

//     // TODO: for slabs that are full, should we forget them?
//     // only remember them when objects within them are freed

//     // get first page address from bin, convert to meta index
//     uint64_t pg = bins[index];
//     while (kernel_heap_meta[index_of_page(pg)].free_list == NULL) {
//         // loop while current slab is full
//         if (kernel_heap_meta[index_of_page(pg)].next == 0) {
//             // if current slab is last, then create new one
//             uint64_t slab_vir = create_slab(index);

//             // and add the newly created slab to the list head
//             kernel_heap_meta[index_of_page(slab_vir)].free_list = slab_vir;
//             kernel_heap_meta[index_of_page(slab_vir)].next = bins[index];
//             bins[index] = slab_vir;

//             // point pg to the virtual address of newly created slab
//             pg = slab_vir;
//             break;
//         }
//         pg = kernel_heap_meta[index_of_page(pg)].next;
//     }

//     // now we found a slab with free object.
//     uint64_t *p = (uint64_t *) kernel_heap_meta[index_of_page(pg)].free_list;
//     kernel_heap_meta[index_of_page(pg)].free_list = *p;

//     return p;
// }

// // we don't need to know the size of the object
// void slab_free(void *addr) {
//     // calculate which page the object in
//     uint64_t pg = (uint64_t) addr & ~(4096UL - 1);

//     // add the newly freed object to the head of the freelist
//     uint64_t *p = (uint64_t *) addr;
//     *p = kernel_heap_meta[index_of_page(pg)].free_list;
//     kernel_heap_meta[index_of_page(pg)].free_list = p;
// }

