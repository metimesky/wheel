#include "page_alloc.h"
#include <env.h>
#include <multiboot.h>
#include <bitmap.h>
#include "utils.h"

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

//extern void raw_write(const char *str, char attr, int pos);

// 初始化页分配器，根据内存布局建立buddy位图
void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // 内核结束的位置就是位图的起始地址，但必须8字节对齐
    buddy_map[0] = (uint64_t *) (((uint64_t) &kernel_bss_end + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1));
    buddy_num[0] = 0;
    
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

// 计算data二进制表示中后缀0的个数
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
    uint64_t idx = find_free_pages(order);
    if (idx) {
        BIT_CLEAR(buddy_map[order], idx);
        return idx << 12;
    }
    for (int new_order = order; new_order < 8; ++new_order) {
        if (idx = find_free_pages(new_order)) {
            while (new_order > order) {
                BIT_CLEAR(buddy_map[new_order], idx);
                idx <<= 1;
                BIT_SET(buddy_map[new_order], idx);
                BIT_SET(buddy_map[new_order], idx+1);
            }
            BIT_CLEAR(buddy_map[new_order], idx);
            return idx << 12;
        }
    }
    return 0;
}

// 回收一个order阶的块
void free_pages(uint64_t addr, int order) {
    addr >>= 12;
    BIT_SET(buddy_map[order], addr);
    for (; order < 7 && BIT_TEST(buddy_map[order], addr^1); ++order) {
        // if two neighbouring block can be merged, then merge them
        BIT_CLEAR(buddy_map[order], addr);
        BIT_CLEAR(buddy_map[order], addr^1);
        addr >>= 1;
        BIT_SET(buddy_map[order+1], addr);
    }
}

/*
// return value of 0 means fail, since physical page 0 is never used
// only alloc continuous pages, doesn't care about mapping.
uint64_t alloc_pages(int order) {
    if (order < 7) {
        // 0~6阶的物理页能从单个uint64_t内部获得
        // when 0 <= order < 8, a single bit can contain that order
        // search in corresponding bitmap for non-zero uint64
        int len = BITS_TO_UINT64(buddy_num[order]);
        int i;
        // find the first non-zero uint64, which contains the bit we want.
        for (i = 0; i < len && buddy_map[order][i] == 0; ++i) {}
        if (i == len) {
            // we are at the end of bitmap, indicates we haven't found any page
            return 0UL;
        } else {
            // we have found a page. The bit is in buddy[level][i], but we have to
            // know the exact (bit) index.
            uint64_t data = buddy_map[order][i];

            // count trailing zeros
            uint64_t addr = BITS_PER_UINT64 * i + __builtin_ctz(data);
            // TODO: marking bitmap to zero can be done in a single for loop, bottom-up
            int index = addr, len = 1;
            // clear bits in this and lower order maps
            for (int l = order; l >= 0; --l) {
                bitmap_clear(buddy_map[l], index, len);
                index <<= 1;
                len <<= 1;
            }
            // clear bits in higher order maps
            index = addr >> 1;
            for (int l = order+1; l < 8; ++l) {
                bitmap_clear(buddy_map[l], index, 1);
                index >>= 1;
            }

            addr <<= 12;    // move address 12 bits left, forming valid page address
            return addr;
        }
    } else if (order < 14) {
        // have to span multi-bits.
        // 1M, 2M, 4M, 8M, 16M, 32M
        // 2   4   8   16  32   64
        //  8   9   10  11  12  13
        // we need to find 2**(order-7) continuous ones.
        uint64_t adder = 1UL;
        uint64_t mask = 0x8000000000000000UL;
        for (int shift = 1 << (order - 7); shift < 64; shift <<= 1) {
            adder |= adder << shift;
            mask  |= mask >> shift;
        }
        switch (order) {
        case 8:     // 1M, 2bits
            adder = 0x5555555555555555UL;
            mask  = 0xaaaaaaaaaaaaaaaaUL;
            // adder |= adder << 2;
            // adder |= adder << 4;
            // adder |= adder << 8;
            // adder |= adder << 16;
            // adder |= adder << 32;
            break;
        case 9:     // 2M, 4bits
            adder = 0x1111111111111111UL;
            mask  = 0x8888888888888888UL;
            // adder |= adder << 4;
            // adder |= adder << 8;
            // adder |= adder << 16;
            // adder |= adder << 32;
            break;
        case 10:    // 4M, 8bits
            adder = 0x0101010101010101UL;
            mask  = 0x8080808080808080UL;
            // adder |= adder << 8;
            // adder |= adder << 16;
            // adder |= adder << 32;
            break;
        case 11:    // 8M, 16bits
            adder = 0x0001000100010001UL;
            mask  = 0x8000800080008000UL;
            // adder |= adder << 16;
            // adder |= adder << 32;
            break;
        case 12:    // 16M, 32bits
            adder = 0x0000000100000001UL;
            mask  = 0x8000000080000000UL;
            // adder |= adder << 32;
            break;
        case 13:    // 32M, 64bits
            adder = 0x0000000000000001UL;
            mask  = 0x8000000000000000UL;
            break;
        }
        int len = BITS_TO_UINT64(buddy_num[7]);
        int i;
        for (i = 0; i < len; ++i) {
            if (buddy_map[7][i] & ~(buddy_map[7][i] + adder) & mask) {
                break;
            }
        }
        if (i == len) {
            // didn't find
            return 0UL;
        } else {
            // we have to know which 
        }
    } else {
        // span multiple uint64.
        // order:   14,  15,   16,   17,   18, 19, 20, 21, 22,  23,  ...
        // memory:  64M, 128M, 256M, 512M, 1G, 2G, 4G, 8G, 16G, 32G, ...
        // #uint64: 2    4     8     16    32  64  128 256 512  1024
        // First we have to check if we have enough memory.
        uint64_t a = 1 << (order - 13); // how many uint64 do we span
        int len = BITS_TO_UINT64(buddy_num[7]);
        int i;
        for (i = 0; i < len; ++i) {
            //
        }
    }
}
*/

