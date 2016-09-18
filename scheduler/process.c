#include <wheel.h>
#include <memory.h>
#include <gdt_tss.h>
#include <interrupt.h>

#include <lib/locking.h>
#include <drivers/console.h>

// 多核的调度比单核复杂很多
// VxWorks中，BSP负责中断处理

// 进程块中应该存放所有进程状态，因此必然包括寄存器信息。
// 也就是说，中断的时候保存的信息，要放在这个结构体之中。
struct pcb {
    uint32_t pid;
    char *name;
    uint64_t resume_rsp;
    uint64_t ustack_top;
    uint64_t kstack_top;    // const
    int_context_t context;
};
typedef struct pcb pcb_t;

struct pcb_list_node {
    pcb_t pcb;
    struct pcb_list_node *next;
};

// static struct pcb_list_node *process_list;
static pcb_t process_list[16];
static int process_count = 0;   // init函数也作为一个任务存在
static int next_pid = -1;

// lock for creating process
static raw_spinlock_t lock = 0;

// 退出ISR时，需要设置正确的RSP，才能恢复到指定的Task状态。
uint64_t target_rsp;

// 目前是创建内核层任务
void create_process(uint64_t entry) {
    raw_spin_lock(&lock);

    uint64_t stack = alloc_pages(0);
    process_list[process_count].kstack_top = KERNEL_VMA + stack + 4096;

    process_list[process_count].context.cs = 0x08;
    process_list[process_count].context.ss = 0x10;
    process_list[process_count].context.rsp = KERNEL_VMA + stack + 4096;
    process_list[process_count].context.rip = entry;
    process_list[process_count].context.rflags = 1UL << 9;  // enable IF

    process_list[process_count].resume_rsp = &(process_list[process_count].context.r15);

    console_print("creating kernel task %d at %x, stack %x.\n", process_count, entry, stack);

    ++process_count;

    raw_spin_unlock(&lock);
}

// 创建一个用户层的任务
void create_process3(uint64_t entry) {
    raw_spin_lock(&lock);

    uint64_t kstack = alloc_pages(0);
    uint64_t ustack = alloc_pages(0);
    process_list[process_count].kstack_top = KERNEL_VMA + kstack + 4096;
    process_list[process_count].ustack_top = KERNEL_VMA + ustack + 4096;

    process_list[process_count].context.cs = 0x18 + 3;
    process_list[process_count].context.ss = 0x20 + 3;
    process_list[process_count].context.rsp = KERNEL_VMA + ustack + 4096;
    process_list[process_count].context.rip = entry;
    process_list[process_count].context.rflags = 1UL << 9;  // enable IF

    process_list[process_count].resume_rsp = &(process_list[process_count].context.r15);

    console_print("creating kernel task %d at %x, kstack %x, ustack %x.\n", process_count, entry, kstack, ustack);

    ++process_count;

    raw_spin_unlock(&lock);
}

// 是否允许在同一CPU内切换进程
static bool preemption = false;

// 在local APIC Timer中调用，在中断环境下执行
void (*clock_isr)(int_context_t *ctx) = NULL;

static void clock_isr_func(int_context_t *ctx) {
    static char *video = (char *) (KERNEL_VMA + 0xa0000);

    if (preemption && process_count > 0) {
        if (next_pid >= 0 && next_pid < process_count) {
            process_list[next_pid].resume_rsp = (uint64_t) ctx;
        }

        ++next_pid;
        if (next_pid >= process_count) {
            next_pid = 0;
        }

        // pop时从这里开始
        target_rsp = process_list[next_pid].resume_rsp;

        // 下一次从ring3中断时，切换到这个位置（通常不需要改变）
        ((tss_t *)PERCPU(tss))->rsp0 = (uint64_t)&(process_list[next_pid].context.r15) + sizeof(int_context_t);
    }
}

void start_schedule() {
    clock_isr = clock_isr_func;
}

void preempt_enable() { preemption = true; }
void preempt_disable() { preemption = false; }