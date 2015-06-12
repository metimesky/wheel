#ifndef __PAGING_H__
#define __PAGING_H__ 1

struct frame {
    uint64_t linear;    // the linear address this frame mapped to
    uint32_t pid;       // the process that is owning this frame
    uint32_t attr;      // present, granularity
} __attribute__((packed));
typedef struct frame frame_t;

void paging_init(uint32_t size, uint32_t mmap_addr, uint32_t mmap_length)

#endif // __PAGING_H__