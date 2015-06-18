#include <env.h>
#include <multiboot.h>
#include <string.h>
#include <bitmap.h>
#include "../memory/page_alloc.h"

void raw_write(const char *str, char attr, int pos) {
    static char* const video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2 * (pos+i)] = str[i];
        video[2 * (pos+i) + 1] = attr;
    }
}

void read_info(uint32_t eax, uint32_t ebx) {
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        raw_write("Bootloader magic number is invalid.", 0x4e, 0);
        while (1) {}
    }

    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
    char buf[64];

    int line = 0;

    // are `mem_*` and `mmap_*` valid?
    if (mbi->flags & (1<<0) && mbi->flags & (1<<6)) {
        raw_write("lower memory (KB):", 0x0f, 80*line);
        raw_write(u32_to_str(mbi->mem_lower, buf, 10), 0x0f, 80*line + 19);
        raw_write("upper memory (KB):", 0x0f, 80*line + 30);
        raw_write(u32_to_str(mbi->mem_upper, buf, 10), 0x0f, 80*line + 49);
        ++line;

        //
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
        while ((uint32_t) mmap < mbi->mmap_addr + mbi->mmap_length) {
            raw_write("Addr:", 0x0d, 80*line);
            raw_write(u64_to_str(mmap->addr, buf, 16), 0x0d, 80*line+6);
            raw_write("Size:", 0x0d, 80*line+30);
            raw_write(u64_to_str(mmap->len, buf, 16), 0x0d, 80*line+36);
            if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
                raw_write("OK", 0x0a, 80*line+78);
            } else {
                raw_write("NO", 0x0c, 80*line+78);
            }
            mmap = (multiboot_memory_map_t *)((uint32_t) mmap + mmap->size + sizeof(uint32_t));
            ++line;
        }

        // init page allocator
        page_alloc_init(mbi->mmap_addr, mbi->mmap_length);
    } else {
        raw_write("Memory information is not accessible.", 0x4e, 0);
        while (1) {}
    }
}

extern char kernel_load_addr;
extern char kernel_end;
extern char kernel_text_start;
extern char kernel_text_end;
extern char kernel_data_start;
extern char kernel_data_end;
extern char kernel_bss_start;
extern char kernel_bss_end;

extern char kernel_stack;
extern char pml4t;

extern uint64_t *buddy_map[8];
extern uint64_t buddy_num[8];

void wheel_main(uint32_t eax, uint32_t ebx) {
    read_info(eax, ebx);

    char buf[128];
    int line = 8;
// /*
    raw_write("kernel start:", 0x0b, 80*line);
    raw_write(u64_to_str((uint64_t) &kernel_load_addr, buf, 16), 0x0b, 80*line+14);
    raw_write("kernel end:", 0x0b, 80*line+40);
    raw_write(u64_to_str((uint64_t) &kernel_end, buf, 16), 0x0b, 80*line+40+12);
    ++line;
    //
    raw_write("text start:", 0x0b, 80*line);
    raw_write(u64_to_str((uint64_t) &kernel_text_start, buf, 16), 0x0b, 80*line+14);
    raw_write("text end:", 0x0b, 80*line+40);
    raw_write(u64_to_str((uint64_t) &kernel_text_end, buf, 16), 0x0b, 80*line+40+12);
    ++line;
    //
    raw_write("data start:", 0x0b, 80*line);
    raw_write(u64_to_str((uint64_t) &kernel_data_start, buf, 16), 0x0b, 80*line+14);
    raw_write("data end:", 0x0b, 80*line+40);
    raw_write(u64_to_str((uint64_t) &kernel_data_end, buf, 16), 0x0b, 80*line+40+12);
    ++line;
    //
    raw_write("bss start:", 0x0b, 80*line);
    raw_write(u64_to_str((uint64_t) &kernel_bss_start, buf, 16), 0x0b, 80*line+14);
    raw_write("bss end:", 0x0b, 80*line+40);
    raw_write(u64_to_str((uint64_t) &kernel_bss_end, buf, 16), 0x0b, 80*line+40+12);
    ++line;
    //
    raw_write("kernel stack:", 0x0b, 80*line);
    raw_write(u64_to_str((uint64_t) &kernel_stack, buf, 16), 0x0b, 80*line+14);
    raw_write("pml4t:", 0x0b, 80*line+40);
    raw_write(u64_to_str((uint64_t) &pml4t, buf, 16), 0x0b, 80*line+40+12);
    ++line;
// */
    sprintf(buf, "hello, world!");
    raw_write(buf, 0x4e, 80*line);
    ++line;
    //
    sprintf(buf, "number is %d", 123654);
    raw_write(buf, 0x4e, 80*line);
    ++line;
    //
    sprintf(buf, "kernel ends at %p", &kernel_bss_end);
    raw_write(buf, 0x4e, 80*line);
    ++line;
    //
    sprintf(buf, "char %c and string %s", 'c', "lorem ipsum");
    raw_write(buf, 0x4e, 80*line);
    ++line;

    for (int order = 0; order < 8; ++order) {
        raw_write(u64_to_str((uint64_t) buddy_map[order], buf, 16), 0x0a, 80*line);
        raw_write(u64_to_str(buddy_num[order], buf, 10), 0x0a, 80*line+30);
        ++line;
    }
}
