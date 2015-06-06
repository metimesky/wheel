#include <types.h>

extern uint64_t kernel_bss_end;

typedef unsigned long long frame_t;

#define USED_4K(f)  ((f) &  1)
#define USED_2M(f)  ((f) &  2)
#define USED_1G(f)  ((f) &  4)
#define PAGE_4K(f)  ((f) &  8)
#define PAGE_2M(f)  ((f) & 16)
#define PAGE_1G(f)  ((f) & 32)
#define VIRTUAL(f)  ((f) & 0xfffffffffffff000)

frame_t *frames = NULL;

// param: size -- the size of physical memory in KB.
void physical_memory_manager_init(int size) {
	// frames start from the end of kernel, align upward to 4KB.
	frames = (frame_t *) (((uint64_t) &kernel_bss_end + 0x1000 - 1) & ~(0x1000 - 1));

	// clear the memory
	for (int i = 0; i < size; ++i) {
		frames[0] = 0L;
	}
}