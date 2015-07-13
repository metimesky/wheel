#include <env.h>
#include <multiboot.h>
#include <string.h>
#include <utils.h>
#include "../memory/page_alloc.h"
#include "interrupt.h"
#include "task.h"

int line = 0;

void raw_write(const char *str, char attr, int pos) {
    static char* const video = (char*) 0xb8000;
    for (int i = 0; str[i]; ++i) {
        video[2 * (pos+i)] = str[i];
        video[2 * (pos+i) + 1] = attr;
    }
}

void test_abi(uint64_t a, uint64_t b, uint64_t c) {
    char buf[128];
    sprintf(buf, "%d %d %d", a, b, c);
    raw_write(buf, 0x0e, 80);
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

extern char kernel_text_start;
extern char kernel_text_end;
extern char kernel_data_start;
extern char kernel_data_end;
extern char kernel_bss_start;
extern char kernel_bss_end;

// this is out TSS segment
tss_t tss_seg;

struct sys_desc {
    uint16_t limit_1;
    uint16_t base_1;
    uint8_t  base_2;
    uint8_t  attr_1;
    uint8_t  attr_2;
    uint8_t  base_3;
    uint32_t base_4;
    uint32_t reserved;
} __attribute__((packed));
typedef struct sys_desc sys_desc_t;

extern uint64_t gdt[];
extern uint16_t get_cs();

void wheel_main(uint32_t eax, uint32_t ebx) {
    // first check if the bootloader is multiboot compliant
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        panic("Bootloader magic number is invalid.");
    }

    char buf[70];
    sprintf(buf, "cs is %d", get_cs());
    raw_write(buf, 0x1e, 0);

    interrupt_init();
    // init TSS
    tss_seg.reserved_1 = 0;
    tss_seg.reserved_2 = 0;
    tss_seg.reserved_3 = 0;
    tss_seg.reserved_4 = 0;
    tss_seg.rsp0 = 0;
    tss_seg.rsp1 = 0;
    tss_seg.rsp2 = 0;
    tss_seg.ist1 = 0;
    tss_seg.ist2 = 0;
    tss_seg.ist3 = 0;
    tss_seg.ist4 = 0;
    tss_seg.ist5 = 0;
    tss_seg.ist6 = 0;
    tss_seg.ist7 = 0;
    tss_seg.iomap_base = 0;

    sys_desc_t *tss_desc = (sys_desc_t *) &gdt[5];
    tss_desc->reserved = 0;
    tss_desc->limit_1 = ((uint64_t) sizeof(tss_seg)) & 0xffff;
    tss_desc->attr_2 = ((uint64_t) sizeof(tss_seg) >> 16) & 0x0f;
    tss_desc->base_1 = ((uint64_t) &tss_seg) & 0xffff;
    tss_desc->base_2 = ((uint64_t) &tss_seg >> 16) & 0xff;
    tss_desc->attr_1 = 0x89;  // Present, available 64-bit TSS
    tss_desc->base_3 = ((uint64_t) &tss_seg >> 24) & 0xff;
    tss_desc->base_4 = ((uint64_t) &tss_seg >> 32) & 0xffffffff;

    load_tr(0x28);
/*
    char buf[128];
    raw_write(u64_to_str((uint64_t) &kernel_text_start, buf, 16), 0x0f, 0);
    raw_write(u64_to_str((uint64_t) &kernel_text_end, buf, 16), 0x0f, 40);
    raw_write(u64_to_str((uint64_t) &kernel_data_start, buf, 16), 0x0f, 80);
    raw_write(u64_to_str((uint64_t) &kernel_data_end, buf, 16), 0x0f, 120);
    raw_write(u64_to_str((uint64_t) &kernel_bss_start, buf, 16), 0x0f, 160);
    raw_write(u64_to_str((uint64_t) &kernel_bss_end, buf, 16), 0x0f, 200);
*/
    multiboot_info_t *mbi = (multiboot_info_t *) ebx;
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);
    __asm__ volatile ("sti");
    // raw_write("after int", 0x1e, 240);
    // raw_write("after int!", 0x1e, 240);
    // raw_write("after int!!", 0x1e, 240);
    // raw_write("after int!!!", 0x1e, 240);
    // raw_write("after int!!!!", 0x1e, 240);
    // raw_write("after int!!!!!", 0x1e, 240);
    // raw_write("after int!!!!!!", 0x1e, 240);
    // raw_write("after int!!!!!!!", 0x1e, 240);
    // raw_write("after int!!!!!!!!", 0x1e, 240);
    // raw_write("after int!!!!!!!!!", 0x1e, 240);
    //
    while (1) {}
}
