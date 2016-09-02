#include <memory.h>
#include <interrupt.h>
#include <multiboot.h>
// #include <lib/cpu.h>  // invlpg
#include <lib/bits.h>

// 内核结束位置的物理地址
uint64_t kernel_end_addr;

// 两个per-CPU Data起始地址之间的偏移
uint64_t __percpu_offset;

extern char percpu_start_addr;
extern char percpu_data_end_addr;
extern char percpu_end_addr;

// 复制.percpu.data和.percpu.bss的内容，该函数必须在page_alloc_init之前执行
void __init percpu_area_init() {
    uint64_t v_percpu_start = (uint64_t)(KERNEL_VMA + &percpu_start_addr);
    uint64_t v_percpu_data_end = (uint64_t) (KERNEL_VMA + &percpu_data_end_addr);
    uint64_t v_percpu_end = (uint64_t)(KERNEL_VMA + &percpu_end_addr);
    
    // per-CPU Data由.percpu.data和.percpu.bss两部分组成，只有.percpu.data的内容需要复制
    uint64_t copy_length = v_percpu_data_end - v_percpu_start;
    
    // per-CPU Data的长度
    __percpu_offset = (v_percpu_end - v_percpu_start + 15UL) & ~15UL;
    
    // 逐一复制per-CPU Data
    for (int i = 1; i < local_apic_count; ++i) {
        memcpy(v_percpu_start + __percpu_offset * i, v_percpu_start, copy_length);
    }
    
    // 标记下当前内核结束位置的物理地址
    kernel_end_addr = (uint64_t)&percpu_end_addr + __percpu_offset * (local_apic_count - 1);
    kernel_end_addr = (kernel_end_addr + 7UL) & ~7UL;
    
}

#define BUDDY_LEVEL 7
static unsigned long *buddy_map[BUDDY_LEVEL];   // 共7阶位图，1表示可用，0表示不可用或已合并为更高阶的buddy
static int buddy_length[BUDDY_LEVEL];           // 各阶位图的比特位数
static uint64_t free_page_count;

// 初始化buddy位图，该函数必须在percpu_area_init之后执行
void __init page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // 0阶位图从当前最末位置开始
    buddy_map[0] = (uint64_t *) (KERNEL_VMA + kernel_end_addr);
    buddy_length[0] = 0;
    
    free_page_count = 0;

    // 遍历mem-map，填充第0阶位图
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)(KERNEL_VMA + mmap_addr);
    uint64_t start, end;
    while ((uint32_t)mmap < (mmap_addr + mmap_length)) {
        if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
            // 获取该内存段的开始和结束地址
            start = mmap->addr;
            end = start + mmap->len;

            // 计算开始和结束位置在位图中的下标（开始地址向上取整，结束地址向下取整）
            start += 4096 - 1;
            start >>= 12;
            end >>= 12;

            // 填充不可用空间和可用空间
            bitmap_clear(buddy_map[0], buddy_length[0], start - buddy_length[0]);
            bitmap_set(buddy_map[0], start, end - start);

            // 记当前段的末下标为位图的总大小
            buddy_length[0] = end;

            // add page count of this zone into total
            free_page_count += end - start + 1;
        }
        mmap = (multiboot_memory_map_t *) ((uint64_t)mmap + mmap->size + sizeof(uint32_t));
    }
    
    // 剩余位清零
    buddy_map[0][buddy_length[0] / BITMAP_BITS_PER_WORD] &= BITMAP_LAST_WORD_MASK(buddy_length[0]);
    
    // 计算各阶位图的位置和长度
    for (int i = 1; i < BUDDY_LEVEL; ++i) {
        buddy_map[i] = buddy_map[i-1] + BITMAP_NUM_WORDS(buddy_length[i-1]);
        buddy_length[i] = (buddy_length[i-1] + 1) >> 1;
    }
    
    // 最后一阶位图结束位置记为内核结束地址。
    kernel_end_addr = (uint64_t)(buddy_map[BUDDY_LEVEL-1] + BITMAP_NUM_WORDS(buddy_length[BUDDY_LEVEL-1]));
    kernel_end_addr -= KERNEL_VMA;
    
    // 在第一阶位图里标记内核使用的内存区域
    uint64_t pages_used = (kernel_end_addr + 4095UL) >> 12;
    bitmap_clear(buddy_map[0], 0, pages_used);
    // TODO: 这里存在漏洞，标记的已使用区域可能包含某些不可用内存，
    // 造成free_pages_count比真实值要小
    free_page_count -= pages_used;
    
    // 循环填充各阶位图
    for (int i = 1; i < BUDDY_LEVEL; ++i) {
        bitmap_clear(buddy_map[i], 0, buddy_length[i]);
        for (int bit = 0; bit < buddy_length[i]; ++bit) {
            // TODO: 现在的做法十分低效，能否通过位运算提速？
            if (bitmap_test(buddy_map[i-1], 2*bit) && bitmap_test(buddy_map[i-1], 2*bit+1)) {
                bitmap_clear(buddy_map[i-1], 2*bit, 1);
                bitmap_clear(buddy_map[i-1], 2*bit+1, 1);
                bitmap_set(buddy_map[i], bit, 1);
            }
        }
    }
}

extern char init_end_addr;

// 回收init section的控件
void reclaim_init_space() {
    ;
}