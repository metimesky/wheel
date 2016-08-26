#include <interrupt.h>
#include <drivers/acpi/acpi.h>
#include <drivers/pit.h>
#include <lib/cpu.h>

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

// Local APIC Vector Table位段
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
#define MODE_FIXED                  0x0         // delivery mode: Fixed
#define MODE_LOWEST                 0x1         // delivery mode: Lowest
#define MODE_SMI                    0x2         // delivery mode: SMI
#define MODE_NMI                    0x4         // delivery mode: NMI
#define MODE_INIT                   0x5         // delivery mode: INIT
#define MODE_STARTUP                0x6         // delivery mode: StartUp
#define STATUS_PEND                 0x1000      // delivery status: Pend

struct local_apic {
    uint64_t base;
    int id;
};
typedef struct local_apic local_apic_t;

// static uint64_t phy_base;   // from MADT

static uint64_t base_addr;
static local_apic_t local_apic_list[16];
int local_apic_count;

// 添加的工作在BSP中执行
void local_apic_add(ACPI_MADT_LOCAL_APIC *local_apic) {
    ++local_apic_count;
}

void local_apic_address_override(ACPI_MADT_LOCAL_APIC_OVERRIDE *override) {
    ;
}

static void local_apic_timer_callback(int vec, interrupt_context_t *ctx) {
    static char *video = (char *)(KERNEL_VMA + 0xa0000);
    ++video[156];
    local_apic_send_eoi();
}

// Local APIC timer的频率与总线频率相关，因此需要使用PIT测量总线速度
static void get_bus_speed() {
    ;
}

void local_apic_timer_init() {
    interrupt_set_handler(LVT_VEC_BASE, local_apic_timer_callback);
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_CONFIG) = LOCAL_APIC_TIMER_DIVBY_16 & LOCAL_APIC_TIMER_DIVBY_MASK;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) |= (1 << 17);
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) &= ~LOCAL_APIC_MASK;
    *(uint32_t *)(base_addr + LOCAL_APIC_TIMER_ICR) = 100;
    // *(uint32_t *)(base_addr + LOCAL_APIC_TIMER) = LOCAL_APIC_TIMER_PERIODIC | (LVT_VEC_BASE & LOCAL_APIC_VECTOR);
}

// Spurious中断的处理函数不需要EOI
static void svr_callback(int vec, interrupt_context_t *ctx) {
    // console_print("SVR Interrupt\n");
}

void local_apic_init() {
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
    // local_apic_list[local_apic_count].base = base;
    // ++local_apic_count;

    // 获取版本和LVT数目
    uint32_t v = *(uint32_t *)(base + LOCAL_APIC_VER) & LOCAL_APIC_VERSION_MASK;
    uint32_t maxLvt = (*(uint32_t *)(base + LOCAL_APIC_VER) & LOCAL_APIC_MAXLVT_MASK) >> 16;

    // reset the DFR, TPR, TIMER_CONFIG, and TIMER_ICR
    *(uint32_t *)(base + LOCAL_APIC_DFR) = 0xffffffff;
    *(uint32_t *)(base + LOCAL_APIC_TPR) = 0x0;
    *(uint32_t *)(base + LOCAL_APIC_TIMER_CONFIG) = 0x0;
    *(uint32_t *)(base + LOCAL_APIC_TIMER_ICR) = 0x0;

    // 设置LVT条目，默认全部禁用
    *(uint32_t *)(base + LOCAL_APIC_TIMER) = LOCAL_APIC_MASK|(LVT_VEC_BASE & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_THERMAL) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+1) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_PMC) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+2) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_LINT0) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+3) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_LINT1) = LOCAL_APIC_MASK | ((LVT_VEC_BASE+4) & LOCAL_APIC_VECTOR);
    *(uint32_t *)(base + LOCAL_APIC_ERROR) = ((LVT_VEC_BASE+5) & LOCAL_APIC_VECTOR);

    // discard a pending interrupt if any
    *(uint32_t *)(base + LOCAL_APIC_EOI) = 1;

    // 设置SVR
    interrupt_set_handler(SVR_VEC_NUM, svr_callback);
    *(uint32_t *)(base + LOCAL_APIC_SVR) = LOCAL_APIC_ENABLE | SVR_VEC_NUM;

    // local_apic_timer_init();
}

void local_apic_send_eoi() {
    *(uint32_t *)(base_addr + LOCAL_APIC_EOI) = 1;
}