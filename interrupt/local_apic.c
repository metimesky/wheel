#include <interrupt.h>
#include <drivers/acpi/acpi.h>
#include <drivers/pit.h>
#include <lib/cpu.h>
#include <drivers/console.h>
// #include <scheduler.h>

// 该模块不仅负责中断相关配置，还有local Apic Timer
// local Apic Timer作为调度器的中断源非常合适，但是不适合作为计时间的时钟，
// 因为节能优化可能改变总线频率

// Local APIC内存映射寄存器的偏移地址
#define LOCAL_APIC_ID           0x0020  // Local APIC ID Reg
#define LOCAL_APIC_VER          0x0030  // Local APIC Version Reg
#define LOCAL_APIC_TPR          0x0080  // Task Priority Reg
#define LOCAL_APIC_APR          0x0090  // Arbitration Priority Reg
#define LOCAL_APIC_PPR          0x00a0  // Processor Priority Reg
#define LOCAL_APIC_EOI          0x00b0  // End of Interrupt Reg
#define LOCAL_APIC_LDR          0x00d0  // Logical Destination Reg
#define LOCAL_APIC_DFR          0x00e0  // Destination Format Reg
#define LOCAL_APIC_SVR          0x00f0  // Spurious Interrupt Reg
#define LOCAL_APIC_ISR          0x0100  // In-service Reg
#define LOCAL_APIC_TMR          0x0180  // Trigger Mode Reg
#define LOCAL_APIC_IRR          0x0200  // Interrupt Request Reg
#define LOCAL_APIC_ESR          0x0280  // Error Status Reg
#define LOCAL_APIC_ICR_L        0x0300  // Interrupt Command Reg
#define LOCAL_APIC_ICR_H        0x0310  // Interrupt Command Reg
#define LOCAL_APIC_TIMER        0x0320  // LVT (Timer)
#define LOCAL_APIC_THERMAL      0x0330  // LVT (Thermal)
#define LOCAL_APIC_PMC          0x0340  // LVT (PMC)
#define LOCAL_APIC_LINT0        0x0350  // LVT (LINT0)
#define LOCAL_APIC_LINT1        0x0360  // LVT (LINT1)
#define LOCAL_APIC_ERROR        0x0370  // LVT (ERROR)
#define LOCAL_APIC_TIMER_ICR    0x0380  // Timer Initial Count Reg
#define LOCAL_APIC_TIMER_CCR    0x0390  // Timer Current Count Reg
#define LOCAL_APIC_TIMER_CONFIG 0x03e0  // Timer Divide Config Reg

// MSR寄存器位段
#define IA32_APIC_BASE              0x1b
#define LOCAL_APIC_BASE_MASK        0xfffff000  // LO APIC Base Addr mask
#define LOCAL_APIC_ENABLE           0x00000800  // LO APIC Global Enable
#define LOCAL_APIC_BSP              0x00000100  // LO APIC BSP

// Local APIC ID寄存器位段
#define LOCAL_APIC_ID_MASK          0x1f000000  // ID [28-24] mask
#define LOCAL_APIC_CLUSTER_ID_MASK  0x18000000  // CLUSTER ID [28-27] mask
#define LOCAL_APIC_PHY_PRC_ID_MASK  0x06000000  // PHYSICAL PROCESSOR ID [26-25] mask
#define LOCAL_APIC_LOG_PRC_ID_MASK  0x01000000  // LOGICAL PROCESSOR ID [24] mask

// Local APIC VERSION寄存器位段
#define LOCAL_APIC_VERSION_MASK     0x000000ff  // LO APIC Version mask
#define LOCAL_APIC_MAXLVT_MASK      0x00ff0000  // LO APIC Max LVT mask
#define LOCAL_APIC_PENTIUM4         0x00000014  // LO APIC in Pentium4
#define LOCAL_APIC_LVT_PENTIUM4     5           // LO APIC LVT - Pentium4
#define LOCAL_APIC_LVT_P6           4           // LO APIC LVT - P6
#define LOCAL_APIC_LVT_P5           3           // LO APIC LVT - P5

// LVT条目及ICR位段
#define LOCAL_APIC_VECTOR           0x000000ff  // vectorNo
#define LOCAL_APIC_MODE             0x00000700  // delivery mode
#define LOCAL_APIC_FIXED            0x00000000  // delivery mode: FIXED
#define LOCAL_APIC_SMI              0x00000200  // delivery mode: SMI
#define LOCAL_APIC_NMI              0x00000400  // delivery mode: NMI
#define LOCAL_APIC_EXT              0x00000700  // delivery mode: ExtINT
#define LOCAL_APIC_IDLE             0x00000000  // delivery status: Idle
#define LOCAL_APIC_PEND             0x00001000  // delivery status: Pend
#define LOCAL_APIC_HIGH             0x00000000  // polarity: High
#define LOCAL_APIC_LOW              0x00002000  // polarity: Low
#define LOCAL_APIC_REMOTE           0x00004000  // remote IRR
#define LOCAL_APIC_EDGE             0x00000000  // trigger mode: Edge
#define LOCAL_APIC_LEVEL            0x00008000  // trigger mode: Level
#define LOCAL_APIC_DEASSERT         0x00000000  // level: de-assert
#define LOCAL_APIC_ASSERT           0x00004000  // level: assert
#define LOCAL_APIC_MASK             0x00010000  // mask

// Local APIC Spurious-Interrupt Register
#define LOCAL_APIC_ENABLE           0x100       // APIC Enabled
#define LOCAL_APIC_FOCUS_DISABLE    0x200       // Focus Processor Checking

// Local APIC Timer
#define LOCAL_APIC_TIMER_DIVBY_2    0x0         // Divide by 2
#define LOCAL_APIC_TIMER_DIVBY_4    0x1         // Divide by 4
#define LOCAL_APIC_TIMER_DIVBY_8    0x2         // Divide by 8
#define LOCAL_APIC_TIMER_DIVBY_16   0x3         // Divide by 16
#define LOCAL_APIC_TIMER_DIVBY_32   0x8         // Divide by 32
#define LOCAL_APIC_TIMER_DIVBY_64   0x9         // Divide by 64
#define LOCAL_APIC_TIMER_DIVBY_128  0xa         // Divide by 128
#define LOCAL_APIC_TIMER_DIVBY_1    0xb         // Divide by 1
#define LOCAL_APIC_TIMER_DIVBY_MASK 0xb         // mask bits
#define LOCAL_APIC_TIMER_PERIODIC   0x00020000  // Timer Mode: Periodic

// Interrupt Command Register: delivery mode and status
#define MODE_FIXED          0x0000      // delivery mode: Fixed
#define MODE_LOWEST         0x0100      // delivery mode: Lowest
#define MODE_SMI            0x0200      // delivery mode: SMI
#define MODE_NMI            0x0400      // delivery mode: NMI
#define MODE_INIT           0x0500      // delivery mode: INIT
#define MODE_STARTUP        0x0600      // delivery mode: StartUp
#define SENDTO_SELF         (1 << 18)
#define SENDTO_ALL          (2 << 18)
#define SENDTO_OTHERS       (3 << 18)
#define STATUS_PEND         0x1000      // delivery status: Pend

struct local_apic {
    uint64_t base;
    uint32_t id;
    uint32_t processor_id;
};
typedef struct local_apic local_apic_t;

// static uint64_t phy_base;   // from MADT

static uint64_t base_addr;
static local_apic_t local_apic_list[16];
int local_apic_count;

// 添加的工作在BSP中执行
void __init local_apic_add(ACPI_MADT_LOCAL_APIC *local_apic) {
    local_apic_list[local_apic_count].id = local_apic->Id;
    local_apic_list[local_apic_count].processor_id = local_apic->ProcessorId;
    ++local_apic_count;
}

void __init local_apic_address_override(ACPI_MADT_LOCAL_APIC_OVERRIDE *override) {
    ;
}

// Spurious中断的处理函数不需要EOI
static void svr_callback(int vec, int_context_t *ctx) {
    // console_print("SVR Interrupt\n");
}

void __init local_apic_init() {
    uint64_t apic_base_msr = read_msr(IA32_APIC_BASE);
    uint64_t base = apic_base_msr & LOCAL_APIC_BASE_MASK;
    if (local_apic_count == 0) {
        // Bootstrap Processor
        apic_base_msr |= LOCAL_APIC_BSP;
    }
    apic_base_msr |= LOCAL_APIC_ENABLE;
    write_msr(IA32_APIC_BASE, apic_base_msr);

    base += KERNEL_VMA;
    base_addr = base;

    // 获取版本和LVT数目
    uint32_t v = *(uint32_t *)(base + LOCAL_APIC_VER) & LOCAL_APIC_VERSION_MASK;
    uint32_t maxLvt = (*(uint32_t *)(base + LOCAL_APIC_VER) & LOCAL_APIC_MAXLVT_MASK) >> 16;

    // reset the DFR, TPR, TIMER_CONFIG, and TIMER_ICR
    *(uint32_t *)(base + LOCAL_APIC_DFR) = 0xffffffff;
    *(uint32_t *)(base + LOCAL_APIC_TPR) = 0x0;
    *(uint32_t *)(base + LOCAL_APIC_TIMER_CONFIG) = 0x0;
    *(uint32_t *)(base + LOCAL_APIC_TIMER_ICR) = 0x0;

    // 设置LVT条目，默认全部禁用
    *(uint32_t *)(base + LOCAL_APIC_TIMER) = LOCAL_APIC_MASK | (LVT_VEC_BASE & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_THERMAL) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+1) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_PMC) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+2) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_LINT0) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+3) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_LINT1) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+4) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_ERROR) = ((LVT_VEC_BASE+5) & LOCAL_APIC_VECTOR);

    // discard a pending interrupt if any
    *(uint32_t *)(base + LOCAL_APIC_EOI) = 1;

    // 设置SVR
    idt_set_int_handler(SVR_VEC_NUM, svr_callback);
    *(uint32_t *)(base + LOCAL_APIC_SVR) = LOCAL_APIC_ENABLE | SVR_VEC_NUM;
}

void local_apic_send_eoi() {
    *(uint32_t *)(base_addr + LOCAL_APIC_EOI) = 1;
}

static DEFINE_PERCPU(uint64_t, local_apic_tick);
extern void (*clock_isr)(int_context_t *ctx);   // 定义在scheduler中

static void local_apic_timer_callback(int vec, int_context_t *ctx) {
    // ++local_apic_tick;
    volatile uint64_t *t = &PERCPU(local_apic_tick);
    ++(*t);

    if (clock_isr) {
        clock_isr(ctx);
    }
    local_apic_send_eoi();
}

static uint32_t local_apic_timer_icr = 0;

void __init local_apic_timer_init() {
    idt_set_int_handler(LVT_VEC_BASE, local_apic_timer_callback);
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_CONFIG) = LOCAL_APIC_TIMER_DIVBY_1 & LOCAL_APIC_TIMER_DIVBY_MASK;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) |= LOCAL_APIC_TIMER_PERIODIC;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) &= ~LOCAL_APIC_MASK;

    // 利用PIT进行同步，计算总线频率
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_ICR) = 0xffffffff;
    uint32_t t1 = *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_CCR);
    pit_delay(1000);
    uint32_t t2 = *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_CCR);
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_ICR) = 0;
    local_apic_timer_icr = (t1 - t2) / 1000;

    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_ICR) = local_apic_timer_icr;
}

void __init local_apic_timer_init_ap() {
    idt_set_int_handler(LVT_VEC_BASE, local_apic_timer_callback);
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_CONFIG) = LOCAL_APIC_TIMER_DIVBY_1 & LOCAL_APIC_TIMER_DIVBY_MASK;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) |= LOCAL_APIC_TIMER_PERIODIC;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) &= ~LOCAL_APIC_MASK;

    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_ICR) = local_apic_timer_icr;
}

void local_apic_delay(int ticks) {
    volatile uint64_t *t = &PERCPU(local_apic_tick);
    uint64_t end_tick = *t + ticks;
    while (*t < end_tick) { }
}

extern char trampoline_start_addr;
extern char trampoline_end_addr;
extern char kernel_stack_top;

// 启动AP过程中传递的参数
uint32_t __init ap_id;
uint64_t __init ap_stack_top;
int __init ap_init_ok;

// 发送处理器间中断，启动其他核心
// TODO: 应该将IPI专门抽象出来，写成单独的函数，
// 然后将启动多核的操作用IPI专用函数重写，放在init.c中
void __init local_apic_start_ap() {
    *(uint8_t *)(KERNEL_VMA + 0x0f) = 0x0a;     // set shutdown code
    *(uint16_t *)(KERNEL_VMA + 0x467) = 0x7c00; // set warm-reset vector

    // 复制代码
    char *src = &trampoline_start_addr + KERNEL_VMA;
    char *dst = (char *) (KERNEL_VMA + 0x7c000);
    int length = &trampoline_end_addr - &trampoline_start_addr;
    memcpy(dst, src, length);

    *(uint32_t *)(base_addr + LOCAL_APIC_ERROR) = 0;

    // 发送INIT
    for (int i = 1; i < local_apic_count; ++i) {
        console_print("Starting AP with apic id %x.....", local_apic_list[i].id);
        ap_id = i;
        ap_stack_top = __percpu_offset * i + kernel_stack_top;
        ap_init_ok = false;
        // *(uint16_t *)(KERNEL_VMA + 0x7c000 + 510) = 2*i;
        // *(uint16_t *)(KERNEL_VMA + (char*)&ap_id) = 2*i;
        
        uint32_t upper32 = (local_apic_list[i].id << 24) & 0xff000000;
        uint32_t lower32 = LOCAL_APIC_LEVEL | LOCAL_APIC_ASSERT | MODE_INIT;
        *(uint32_t *)(base_addr + LOCAL_APIC_ICR_H) = upper32;
        *(uint32_t *)(base_addr + LOCAL_APIC_ICR_L) = lower32;

        // console_print("err %x\n", *(uint32_t *)(base_addr + LOCAL_APIC_ERROR));
        // *(uint32_t *)(base_addr + LOCAL_APIC_ERROR) = 0;

        local_apic_delay(10);
        // console_print("2\n");

        // 发送Startup-IPI，向量号是初始化代码的实模式地址
        lower32 = LOCAL_APIC_LEVEL | LOCAL_APIC_ASSERT | MODE_STARTUP | 0x7c;
        *(uint32_t *)(base_addr + LOCAL_APIC_ICR_L) = lower32;

        // console_print("err %x\n", *(uint32_t *)(base_addr + LOCAL_APIC_ERROR));
        // *(uint32_t *)(base_addr + LOCAL_APIC_ERROR) = 0;

        local_apic_delay(10);
        // console_print("3\n");

        // 再次发送Startup-IPI
        *(uint32_t *)(base_addr + LOCAL_APIC_ICR_L) = lower32;

        // console_print("err %x\n", *(uint32_t *)(base_addr + LOCAL_APIC_ERROR));
        // *(uint32_t *)(base_addr + LOCAL_APIC_ERROR) = 0;
        while (!ap_init_ok) { }
        console_print("Done.\n");
    }
}