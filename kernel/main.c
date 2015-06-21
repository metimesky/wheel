#include <env.h>
#include <multiboot.h>
#include <string.h>
#include <bitmap.h>
#include "../memory/page_alloc.h"

int line = 0;

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

    // are `mem_*` and `mmap_*` valid?
    if (mbi->flags & (1<<0) && mbi->flags & (1<<6)) {
        // check lower and upper memory bounds
        raw_write("lower memory (KB):", 0x0f, 80*line);
        raw_write(u32_to_str(mbi->mem_lower, buf, 10), 0x0f, 80*line + 19);
        raw_write("upper memory (KB):", 0x0f, 80*line + 30);
        raw_write(u32_to_str(mbi->mem_upper, buf, 10), 0x0f, 80*line + 49);
        ++line;

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
}
