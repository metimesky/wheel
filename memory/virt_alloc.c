#include "virt_alloc.h"
#include <stdhdr.h>
#include <util.h>
#include "../kernel/fake_console.h"

/* In boot.asm, we initiated 4GB identical mapping.
 * when mapping newly allocated pages, map it to higher virtual addresses.
 */

// defined in boot.asm
extern uint64_t pml4t[512];

// check if the page entry is present
static bool is_page_entry_valid(uint64_t entry) {
    return entry & 1;
}

// get the address field from the entry
static uint64_t get_address(uint64_t entry) {
    return entry & 0x000ffffffffffe00UL;
}

char buf[33];

// map the physical address `frame` to virtual address `page`.
bool map(uint64_t frame, uint64_t page) {
    // first calculate all page entry indexes
    size_t pml4e_index = (page >> 39) & 0x01ffUL;   // page-map level-4 entry index
    size_t pdpe_index  = (page >> 30) & 0x01ffUL;   // page-directory-pointer entry index
    size_t pde_index   = (page >> 21) & 0x01ffUL;   // page-directory entry index
    size_t pte_index   = (page >> 12) & 0x01ffUL;   // page-table entry index

    // pml4t is always present

    // find page-directory-pointer table
    uint64_t pml4e = pml4t[pml4e_index];
    uint64_t *pdpt = (uint64_t *) get_address(pml4e);
    if (!is_page_entry_valid(pml4e)) {
        // if the target pdp not exist, then create it
        pdpt = (uint64_t *) alloc_pages(0); // alloc 4K for page table
        if (0 == pdpt) {
            // if we can't alloc page for pdpt, then report error
            return false;
        }
        memset(pdpt, 0UL, 4096);            // making all entries invalid

        // create pml4 entry for pdp
        pml4t[pml4e_index] = ((uint64_t) pdpt & 0x000ffffffffffe00UL) | 3;   // P, RW
    }

    // find page-directory table
    uint64_t pdpe = pdpt[pdpe_index];
    uint64_t *pdt = (uint64_t *) get_address(pdpe);
    if (!is_page_entry_valid(pdpe)) {
        // if the page-directory does not exist, then create it
        pdt = (uint64_t *) alloc_pages(0);  // 4K
        if (0 == pdt) {
            return false;
        }
        memset(pdt, 0UL, 4096);

        // create pdp entry for pd
        pdpt[pdpe_index] = ((uint64_t) pdt & 0x000ffffffffffe00UL) | 3;     // P, RW
    }

    // find page table
    uint64_t pde = pdt[pde_index];
    uint64_t *pt = (uint64_t *) get_address(pde);
    if (!is_page_entry_valid(pde)) {
        // if the page table not exist, then create it
        pt = (uint64_t *) alloc_pages(0);   // 4K
        if (0 == pt) {
            return false;
        }
        memset(pt, 0UL, 4096);
        pdt[pde_index] = ((uint64_t) pt & 0x000ffffffffffe00UL) | 3;    // P, RW
    }

    // fill page table entry
    if (is_page_entry_valid(pt[pte_index])) {
        // we are mapping this page, and this entry should not be valid
        print("page ");
        print(u32_to_str(page, buf, 16));
        println(" should not valid!");
    }
    pt[pte_index] = (frame & 0x000ffffffffffe00UL) | 3; // P, RW
    invlpg(page);

    return true;
}

// for unmap, we only clear the page entry,
// don't care about other thing.
void unmap(uint64_t page) {
    // first calculate all page entry indexes, just like `map`
    size_t pml4e_index = (page >> 39) & 0x01ffUL;   // page-map level-4 entry index
    size_t pdpe_index  = (page >> 30) & 0x01ffUL;   // page-directory-pointer entry index
    size_t pde_index   = (page >> 21) & 0x01ffUL;   // page-directory entry index
    size_t pte_index   = (page >> 12) & 0x01ffUL;   // page-table entry index
    
    uint64_t *pdpt = (uint64_t *) get_address(pml4t[pml4e_index]);
    uint64_t *pdt  = (uint64_t *) get_address(pdpt[pdpe_index]);
    uint64_t *pt   = (uint64_t *) get_address(pdt[pde_index]);
    pt[pte_index] = 0UL;
    invlpg(page);
}

void virt_alloc_init() {
    uint32_t a, d;
    // cpuid()
}

void test_mm() {
    print("asking for order 0 -> 0x");
    uint64_t addr1 = alloc_pages(0);
    println(u64_to_str(addr1, buf, 16));

    map(addr1, 0x100000000UL);
    char *pointee = (char *) 0x100000000UL;
    char *msg = "This is the string that is written to the newly allocated memory.";

    for (int i = 0; msg[i]; ++i) {
        pointee[i] = msg[i];
    }

    println(pointee);

    map(addr1, 0x200000000UL);
    char *g8 = (char *) 0x200000000UL;
    println(g8);

    unmap(g8);
    unmap(pointee);
}