#include <env.h>
#include <multiboot.h>
#include <string.h>
#include <bitmap.h>
#include <utils.h>
#include "../memory/page_alloc.h"

int line = 0;

void raw_write(const char *str, char attr, int pos) {
    static char* const video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2 * (pos+i)] = str[i];
        video[2 * (pos+i) + 1] = attr;
    }
}

/*
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
*/

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

void handler() {
    raw_write("INTERRUPT!", 0x1e, 23*80);
    while (1) {}
}

#define INT_NUM 128
idt_entry_t idt[INT_NUM];
idt_ptr_t idtr;

// TODO: this function should be moved to its own file
void interrupt_init() {
    // create idt
    idtr.base = (uint64_t) idt;
    idtr.limit = INT_NUM * sizeof(idt_entry_t) - 1;
    for (int i = 0; i < INT_NUM; ++i) {
        idt[i].selector = 8;
        idt[i].offset_low = ((uint64_t) &handler) & 0xffff;
        idt[i].offset_mid = ((uint64_t) &handler >> 16) & 0xffff;
        idt[i].offset_high = ((uint64_t) &handler >> 32) & 0xffffffff;
        idt[i].attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    }
    load_idtr(&idtr);
}

extern char kernel_text_start;
extern char kernel_text_end;
extern char kernel_data_start;
extern char kernel_data_end;
extern char kernel_bss_start;
extern char kernel_bss_end;

void wheel_main(uint32_t eax, uint32_t ebx) {
/*
    char buf[128];
    raw_write(u64_to_str((uint64_t) &kernel_text_start, buf, 16), 0x0f, 0);
    raw_write(u64_to_str((uint64_t) &kernel_text_end, buf, 16), 0x0f, 40);
    raw_write(u64_to_str((uint64_t) &kernel_data_start, buf, 16), 0x0f, 80);
    raw_write(u64_to_str((uint64_t) &kernel_data_end, buf, 16), 0x0f, 120);
    raw_write(u64_to_str((uint64_t) &kernel_bss_start, buf, 16), 0x0f, 160);
    raw_write(u64_to_str((uint64_t) &kernel_bss_end, buf, 16), 0x0f, 200);
*/
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        panic("Bootloader magic number is invalid.");
    }
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);
    interrupt_init();
    //
    //while (1) {}
}
