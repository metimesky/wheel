#include "virt_alloc.h"
#include <stdhdr.h>
#include <util.h>
#include "../kernel/fake_console.h"

void virt_alloc_init() {
    uint32_t a, d;
    // cpuid()
}

/* In kernel, it is a common task to create and delete kernel objects, and
 * this operation is very frequent. we can make an object pool, except we only
 * distinguish object by its size.
 * The idea is alloc memory for a lot of object at one time, and keep track of
 * how many we've used, instead of allocate one whenever we need a new one.
 */

/* Memory is dynamically allocated in block. Each block contains block tag and
 * payload. Block-tag is 16-byte long, and block is 16-byte aligned, payload
 * also 16-byte aligned.
 */

block_tag_t *first_block;

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