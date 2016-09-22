#include <wheel.h>
#include <multiboot.h>
#include <interrupt.h>
#include <syscall.h>
#include <scheduler.h>
#include <gdt_tss.h>
#include <memory.h>
#include <percpu.h>

#include <drivers/console.h>
#include <drivers/acpi/acpi.h>
#include <drivers/pit.h>
#include <drivers/kb.h>

#include <lib/string.h>
#include <lib/locking.h>
#include <lib/printf.h>


extern void local_apic_start_ap();

static __init bool parse_madt() {
    ACPI_TABLE_MADT *madt = NULL;
    if (ACPI_FAILURE(AcpiGetTable(ACPI_SIG_MADT, 1, &madt))) {
        console_print("Cannot locate MADT!\n");
        return false;
    }

    io_apic_count = 0;
    local_apic_count = 0;

    uint8_t *end = (uint8_t *) madt + madt->Header.Length;
    uint8_t *p = (uint8_t *) madt + sizeof(ACPI_TABLE_MADT);
    while (p < end) {
        switch (((ACPI_SUBTABLE_HEADER *) p)->Type) {
        case ACPI_MADT_TYPE_IO_APIC:
            io_apic_add((ACPI_MADT_IO_APIC *) p);
            break;
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
            io_apic_interrupt_override((ACPI_MADT_INTERRUPT_OVERRIDE *) p);
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC:
            local_apic_add((ACPI_MADT_LOCAL_APIC *) p);
            break;
        case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
            local_apic_address_override((ACPI_MADT_LOCAL_APIC_OVERRIDE *) p);
            break;
        case ACPI_MADT_TYPE_NMI_SOURCE:             break;
        case ACPI_MADT_TYPE_LOCAL_APIC_NMI:         break;
        case ACPI_MADT_TYPE_IO_SAPIC:               break;
        case ACPI_MADT_TYPE_LOCAL_SAPIC:            break;
        case ACPI_MADT_TYPE_INTERRUPT_SOURCE:       break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC:           break;
        case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI:       break;
        case ACPI_MADT_TYPE_GENERIC_INTERRUPT:      break;
        case ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR:    break;
        case ACPI_MADT_TYPE_GENERIC_MSI_FRAME:      break;
        case ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR:  break;
        case ACPI_MADT_TYPE_GENERIC_TRANSLATOR:     break;
        case ACPI_MADT_TYPE_RESERVED:               break;
        default:    break;
        }
        p += ((ACPI_SUBTABLE_HEADER *) p)->Length;
    }

    return true;
}

static raw_spinlock_t spin_print = 0;
#define DELAY_TIME 1000

static void kernel_task_A() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    int counter = 0;
    while (1) {
        raw_spin_lock(&spin_print);
        console_print("KA%d.", counter++);
        raw_spin_unlock(&spin_print);
        for (int i = 0; i < DELAY_TIME; ++i) {
            for (int j = 0; j < 100; ++j) {
                video[0] = video[0];
            }
        }
    }
}

static void kernel_task_B() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    int counter = 0;
    while (1) {
        raw_spin_lock(&spin_print);
        console_print("KB%d.", counter++);
        raw_spin_unlock(&spin_print);
        for (int i = 0; i < DELAY_TIME; ++i) {
            for (int j = 0; j < 100; ++j) {
                video[0] = video[0];
            }
        }
    }
}

void user_task_A() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    char buf[32];
    int counter = 0;
    while (1) {
        _sprintf(buf, "UA%d.", counter++);
        raw_spin_lock(&spin_print);
        sys_print(buf);
        raw_spin_unlock(&spin_print);
        for (int i = 0; i < DELAY_TIME; ++i) {
            for (int j = 0; j < 100; ++j) {
                video[0] = video[0];
            }
        }
    }
}

void user_task_B() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    char buf[32];
    int counter = 0;
    while (1) {
        _sprintf(buf, "UB%d.", counter++);
        raw_spin_lock(&spin_print);
        sys_print(buf);
        raw_spin_unlock(&spin_print);
        for (int i = 0; i < DELAY_TIME; ++i) {
            for (int j = 0; j < 100; ++j) {
                video[0] = video[0];
            }
        }
    }
}

extern uint64_t kernel_end_addr;

// 创建进程的时候，入口点是在内核模式下进入的
extern void create_process(uint64_t entry);
extern void create_process3(uint64_t entry);

extern void start_schedule();
extern void syscall_init();

// BSP的初始化函数，由boot.asm调用
void init(uint32_t eax, uint32_t ebx) {
    console_init();
    console_print("Initializing Wheel OS!\n");

    // 检查magic number
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        console_print("Multiboot magic not compliant!\n");
        return;
    }

    multiboot_info_t *mbi = (multiboot_info_t *)(KERNEL_VMA + ebx);

    // early ACPI table access
    if (!initialize_acpi_tables()) {
        console_print("ACPI not available!\n");
        return;
    }

    // 解析MADT
    if (!parse_madt()) {
        console_print("MADT not available!\n");
        return;
    }

    // 初始化IDT和int_handler_table
    idt_init();

    // PIC and APIC
    pic_init();         // 初始化PIC，默认禁用全部IRQ
    io_apic_init();     // 初始化IO APIC，默认禁用全部GSI
    local_apic_init();  // 初始化Local APIC，默认禁用全部LVT，启用SVR

    // kb_init();
    // __asm__ __volatile__("sti");
    // io_apic_unmask_irq(1);
    // while (1) { }
        
    // copying per-cpu data
    percpu_area_init();

    // prepare real gdt and tss
    gdt_init();
    tss_init();

    // 初始化物理内存分配器（最后一个使用kernel_end_addr）
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);

    // 初始化并启用PIT时钟
    pit_init();
    io_apic_unmask_irq(0);

    // 初始化并启用PS/2键盘
    kb_init();
    io_apic_unmask_irq(1);

    __asm__ __volatile__("sti");

    console_print("Initializing Local APIC Timer\n");
    local_apic_timer_init();
    io_apic_mask_irq(0);

    // 安装syscall的ISR
    syscall_init();

    console_print("Waking up all processors\n");
    local_apic_start_ap();

    // 向run_queue中添加进程，当下一次local APIC timer中断触发时，就会自动切换到这些进程中开始执行
    create_process(kernel_task_A);
    create_process(kernel_task_B);
    create_process3(user_task_A);
    create_process3(user_task_B);

    start_schedule();
    // preempt_enable();

    while (true) { }
}

// AP的初始化函数，由trampoline.asm调用
void ap_init(int id) {
    // 加载GDT之后会将GS.BASE清零，在读取per-CPU Data时需要GS.BASE
    gdt_load();
    write_gsbase(id);
    
    idt_load();
    local_apic_init();

    tss_init();

    local_apic_timer_init_ap();
    console_print("Processor %d started.", id);

    while (true) { }
}
