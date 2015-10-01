#include <stdhdr.h>
#include <multiboot.h>
#include <util.h>
#include "page_alloc.h"
#include "../kernel/fake_console.h"

/* 物理内存管理（buddy算法）
 * CPU的分页机制，将内存划分为若干4K的物理页。在buddy算法中，4K大小的物理页也称为0阶块。
 * buddy算法会将两个相邻的0阶块合并为一个1阶块。1阶块是一个大小8K、且8K对齐的物理内存区域。
 * 依此类推，一个n阶块就是连续2^n个4K物理页，总大小(2^n)*4K，并且起始地址是(2^n)*4K的整数倍。
 * * * * * *
 * 这里采用位图来标记各阶块的可用性。如果某个块已分配或不可读写，就将这个二进制位置为1，反之为0。
 * 当操作系统需要分配一个块，就在位图中寻找0，找到之后，将这一位设为1，计算出对应物理页的地址并返回；
 * 释放某个块，就计算出这个块在位图中的下标，并在位图中置该位为0。
 */

// 定义在linker.lds中，它们的地址是内核的开始/结束位置（不包含buddy_map）
extern char kernel_load_addr;
extern char kernel_bss_end;

// 0~7阶位图，覆盖4K到512K的粒度
uint64_t *buddy_map[8];

// 表示各阶buddy_map的长度，存储的是有效二进制位的个数，而非数组中uint64_t的个数
uint64_t buddy_num[8];

// 初始化页分配器，根据内存布局建立buddy位图
void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // 内核结束的位置就是位图的起始地址，但必须8字节对齐
    buddy_map[0] = (uint64_t *) (((uint64_t) &kernel_bss_end + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1));
    buddy_num[0] = 0;

    char buf[33];
    print("creating buddy at 0x");
    println(u32_to_str(buddy_map[0], buf, 16));
    
    // 循环遍历内存段描述符，该信息由引导器GRUB提供
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;
    int start = -1, end = -1;
    while ((uint32_t) mmap < (mmap_addr + mmap_length)) {
        if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type) {
            // 获取该内存段的开始和结束地址
            start = mmap->addr;
            end = start + mmap->len;

            // 计算开始和结束位置在位图中的下标（开始地址向上取整，结束地址向下取整）
            start += 4096 - 1;
            start >>= 12;
            end >>= 12;

            // 填充不可用空间和可用空间
            bitmap_clear(buddy_map[0], buddy_num[0], start - buddy_num[0]);
            bitmap_set(buddy_map[0], start, end - start);

            // 记当前段的末下标为位图的总大小
            buddy_num[0] = end;
        }
        mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }

    // 将位图的剩余位补零，凑齐整数个uint64_t单元
    buddy_map[0][buddy_num[0] / BITS_PER_UINT64] &= BITMAP_LAST_UINT64_MASK(buddy_num[0]);
    
    // 计算各阶位图的位置和长度
    for (int i = 1; i < 8; ++i) {
        buddy_map[i] = buddy_map[i-1] + BITS_TO_UINT64(buddy_num[i-1]);
        buddy_num[i] = (buddy_num[i-1] + 1) >> 1;
    }
    
    // 在第一阶位图内标记内核使用的内存区域
    uint64_t kernel_size = (uint64_t) (buddy_map[7] + BITS_TO_UINT64(buddy_num[7]));
    bitmap_clear(buddy_map[0], 0, (kernel_size + 4096 - 1) >> 12);
    
    // 循环填充各阶位图
    for (int i = 1; i < 8; ++i) {
        bitmap_zero(buddy_map[i], buddy_num[i]);
        for (int bit = 0; bit < buddy_num[i]; ++bit) {
            // TODO: 现在的做法十分低效，能否通过位运算提速？
            if (BIT_TEST(buddy_map[i-1], 2*bit) && BIT_TEST(buddy_map[i-1], 2*bit+1)) {
                BIT_CLEAR(buddy_map[i-1], 2*bit);
                BIT_CLEAR(buddy_map[i-1], 2*bit+1);
                BIT_SET(buddy_map[i], bit);
            }
        }
    }
}

// 计算data二进制表示中后缀0的个数，使用二分查找的方法
static uint64_t count_trailing_zeros(uint64_t data) {
    uint64_t count = 0;
    if (!(data & ((1UL << 32) - 1))) { data >>= 32; count += 32; }
    if (!(data & ((1UL << 16) - 1))) { data >>= 16; count += 16; }
    if (!(data & ((1UL <<  8) - 1))) { data >>=  8; count +=  8; }
    if (!(data & ((1UL <<  4) - 1))) { data >>=  4; count +=  4; }
    if (!(data & ((1UL <<  2) - 1))) { data >>=  2; count +=  2; }
    if (!(data & ((1UL <<  1) - 1))) { data >>=  1; count +=  1; }
    return count;
}

// 寻找一个 `order` 阶的内存块，返回这个块在位图中的下标，而非地址
// 如果找不到符合条件的内存块，则返回零。
// 该函数仅寻找合适的内存块，并不去分配，也不会破坏更大块的可用性。
// 如果该函数返回零，不表示内存不足，仅表示没有最合适的空闲内存块，但程序可以申请更大的块，
// 并将其分裂为若干小的可用块，然后使用其中一个。
uint64_t find_free_pages(int order) {
    if (order < 8) {
        // 在对应位图中寻找非全零的单元
        int len = BITS_TO_UINT64(buddy_num[order]);
        for (int i = 0; i < len; ++i) {
            if (buddy_map[order][i]) {
                return BITS_PER_UINT64 * i + count_trailing_zeros(buddy_map[order][i]);
            }
        }
        return 0;
    }
}

// 分配一个 `order` 阶的块，返回起始物理地址，若找不到则返回0
uint64_t alloc_pages(int order) {
    // 首先寻找最合适大小的空闲内存块
    uint64_t idx = find_free_pages(order);
    if (idx) {
        BIT_CLEAR(buddy_map[order], idx);
        return (idx << order) << 12;
    }

    // 如果没有找到，就寻找更大的内存块
    for (int new_order = order+1; new_order < 8; ++new_order) {
        idx = find_free_pages(new_order);
        if (idx) {
            // 如果找到了空闲块，不断将其二分，直到划分到预期粒度为止
            while (new_order > order) {
                // 将这一阶的内存块标记为不可用
                BIT_CLEAR(buddy_map[new_order], idx);
                idx <<= 1;
                --new_order;
                BIT_SET(buddy_map[new_order], idx);
                BIT_SET(buddy_map[new_order], idx+1);
            }
            BIT_CLEAR(buddy_map[new_order], idx);
            return (idx << order) << 12;
        }
    }
    return 0;
}

// 回收一个order阶的块
void free_pages(uint64_t addr, int order) {
    addr >>= 12;
    addr >>= order;
    BIT_SET(buddy_map[order], addr);
    for (; order < 7 && BIT_TEST(buddy_map[order], addr^1); ++order) {
        // if two neighbouring block can be merged, then merge them
        BIT_CLEAR(buddy_map[order], addr);
        BIT_CLEAR(buddy_map[order], addr^1);
        addr >>= 1;
        BIT_SET(buddy_map[order+1], addr);
    }
}
