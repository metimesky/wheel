#include "page_alloc.h"
#include <env.h>
#include <multiboot.h>
#include <bitmap.h>

/* 物理内存管理（buddy算法）
 * CPU的分页机制，将内存划分为若干4K的物理页。在buddy算法中，4K大小的物理页也称为0阶块。
 * buddy算法会将两个相邻的0阶块合并为一个1阶块。1阶块是一个大小8K、且8K对齐的物理内存区域。
 * 依此类推，一个n阶块就是连续2^n个4K物理页，总大小(2^n)*4K，并且起始地址是(2^n)*4K的整数倍。
 * * * * * *
 * 这里采用位图来标记各阶块的可用性。如果某个块已分配或不可读写，就将这个二进制位置为1，反之为0。
 * 当操作系统需要分配一个块，就在位图中寻找0，找到之后，将这一位设为1，计算出对应物理页的地址并返回；
 * 释放某个块，就计算出这个块在位图中的下标，并在位图中置该位为0。
 * * * * * *
 * 传统buddy算法的实现中，有多少阶的块就需要多少个位图，n+1阶位图包含n阶位图一半的二进制位。
 * 然而这里不采用这种办法。由于位图是通过uint64_t实现的，单次内存访问可以操作64个连续二进制位，
 * 也就是64个buddy。因此，操作2、4、8、16、32、64个连续buddy都可以在一个uint64_t中实现。
 * 因此，使用一个位图就可以表示许多阶的块。
 * 虽然只用了一个位图，但思想上遵从buddy算法。
 */

// 定义在linker.lds中，它们的地址是内核的开始/结束位置（不包含buddy_map）
extern char kernel_load_addr;
extern char kernel_bss_end;

// 0~6阶位图，每一位表示4K物理内存的可用性，64位表示256K物理内存的可用性
uint64_t *buddy_map;

// 表示buddy_map的长度，存储的是有效二进制位的个数，而非数组中uint64_t的个数
uint64_t buddy_num;

// 初始化页分配器，根据内存布局建立buddy位图
void page_alloc_init(uint32_t mmap_addr, uint32_t mmap_length) {
    // 内核结束的位置就是位图的起始地址，但必须8字节对齐
    buddy_map = (uint64_t *) (((uint64_t) &kernel_bss_end + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1));
    buddy_num = 0;
    
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
            bitmap_clear(buddy_map, buddy_num, start - buddy_num);
            bitmap_set(buddy_map, start, end - start);

            // 记当前段的末下标为位图的总大小
            buddy_num = end;
        }
        mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }

    // 将位图的剩余位补零，凑齐整数个uint64_t单元
    buddy_map[buddy_num / BITS_PER_UINT64] &= BITMAP_LAST_UINT64_MASK(buddy_num);
    
    // 标记内核使用的内存区域
    uint64_t kernel_size = (uint64_t) (buddy_map + BITS_TO_UINT64(buddy_num));
    bitmap_clear(buddy_map, (kernel_size + 4096 - 1) >> 12);
}

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
/*
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
*/
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

void free_pages(uint64_t addr, int order) {
}

