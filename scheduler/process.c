#include <wheel.h>
#include <memory.h>
#include <gdt_tss.h>
#include <interrupt.h>

// 多核的调度比单核复杂很多
// VxWorks中，BSP负责中断处理

// 进程块中应该存放所有进程状态，因此必然包括寄存器信息。
// 也就是说，中断的时候保存的信息，要放在这个结构体之中。
struct pcb {
    uint32_t pid;
    char *name;
    uint64_t stack_addr;
    int_context_t context;
};
typedef struct pcb pcb_t;

struct pcb_list_node {
    pcb_t pcb;
    struct pcb_list_node *next;
};

// static struct pcb_list_node *process_list;
static pcb_t process_list[16];
static int process_count;
static int next_pid;

// 退出ISR时，需要设置正确的RSP，才能恢复到指定的Task状态。
uint64_t kernel_rsp;

// 目前是创建内核层任务
void create_process(uint64_t entry) {
    uint64_t stack = alloc_pages(0);
    process_list[process_count].stack_addr = stack;
    process_list[process_count].context.cs = 0x08;
    process_list[process_count].context.ss = 0x10;
    process_list[process_count].context.rsp = KERNEL_VMA + stack + 4096;
    process_list[process_count].context.rip = entry;
    process_list[process_count].context.rflags = 1UL << 9;  // enable IF
    ++process_count;
}

// 在local APIC Timer中调用，在中断环境下执行
void clock_isr() {
    static char *video = (char *) (KERNEL_VMA + 0xa0000);
    ++video[144];

    if (process_count > 0) {
        ++next_pid;
        if (next_pid >= process_count) {
            next_pid = 0;
        }

        // pop时从这里开始
        kernel_rsp = &(process_list[next_pid].context.r15);

        // 下一次中断时，切换到这个位置（通常不需要改变）
        ((tss_t *)PERCPU(tss))->rsp0 = kernel_rsp + sizeof(int_context_t);
    }
}