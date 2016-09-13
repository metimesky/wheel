#include <wheel.h>
#include <multiboot.h>
#include <interrupt.h>
#include <gdt_tss.h>
#include <memory.h>
#include <percpu.h>

#include <drivers/console.h>
#include <drivers/acpi/acpi.h>
#include <drivers/pit.h>

#include <lib/string.h>
#include <lib/locking.h>

#include <syscall.h>

extern void gdt_init();

// 多核初始化
// GDT、分页、IDT等配置对于每个核心都要执行，因此最好等到所有核心都启动后再初始化

// 中断，需要计划好IDT每个条目的作用

// SMP需要知道机器上各CPU的信息，特别是localApicId和cpuId。
// 这两个Id不一定连续，因此不宜作为数组的索引。
// Wheel将MADT中localApic在表中出现顺序作为索引，创建一个单独的local_apic_list表，里面保存localApicId和cpuId
// 每个CPU的索引，称作ProcessorId，保存在MSR寄存器GS.BASE中。

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

static raw_spinlock_t lock = 0;

// 该函数在ring3环境下开始执行
void ring3() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    for (int i = 0; i < 1000; ++i) {
        //raw_spin_lock(&lock);
        //console_print("3");
        video[4*i] = '3';
        video[4*i+1] = 0x3e;
        for (int a = 0; a < 100; ++a) {
            for (int b = 0; b < 1000; ++b) {
                video[0] = video[0];
            }
        }
    }

    while (1) { }
}
extern void syscall_test();
extern void syscall_test2();
void ring32() {
    // __asm__ __volatile__("sti");
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    for (int i = 0; i < 1000; ++i) {
        //raw_spin_lock(&lock);
        //console_print("3");
        // video[4*i+2] = '2';
        // video[4*i+3] = 0x4e;
        for (int a = 0; a < 1000; ++a) {
            for (int b = 0; b < 100; ++b) {
                video[0] = video[0];
            }
        }
        sys_print("2");
        //raw_spin_unlock(&lock);
        // pit_delay(5000);
    }

    while (1) { }
}

// 第一个进程的内核入口点
extern void goto_ring3(uint64_t rsp);
extern void goto_ring32(uint64_t rsp);
static void process0_entry() {
    // 进程刚刚创建的时候，只有内核的上下文，只有内核栈，需要为自己申请专门的用户栈。
    uint64_t p = alloc_pages(0);
    // console_print("allocated page order 0 at %x\n", p);

    // 进入用户态执行，该函数不应该返回
    goto_ring3(KERNEL_VMA + p + 4096);
    while (1) {}
}
static void process1_entry() {
    // 进程刚刚创建的时候，只有内核的上下文，只有内核栈，需要为自己申请专门的用户栈。
    uint64_t p = alloc_pages(0);
    console_print("allocated page order 0 at %x\n", p);

    // 进入用户态执行，该函数不应该返回
    goto_ring32(KERNEL_VMA + p + 4096);
    while (1) {}
}

extern uint64_t kernel_end_addr;

// 创建进程的时候，入口点是在内核模式下进入的
extern void create_process(uint64_t entry);
extern void create_process3(uint64_t entry);

static void process_A() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    int counter = 0;
    while (1) {
        console_print("A%d ", counter++);
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                video[0] = video[0];
            }
        }
        syscall_test();
    }
}

static void process_B() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    int counter = 0;
    while (1) {
        console_print("B%d ", counter++);
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                video[0] = video[0];
            }
        }
    }
}

static void process_C() {
    char *video = (char*) (KERNEL_VMA + 0xa0000);
    int counter = 0;
    while (1) {
        console_print("C%d ", counter++);
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                video[0] = video[0];
            }
        }
    }
}

// void syscall_handler(int n, int_context_t *ctx) {
//     console_print("syscall#%d", ctx->rax);
// }

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
    console_print("Initializing IDT\n");
    idt_init();

    console_print("Initializing PIC and APIC\n");
    pic_init();         // 初始化PIC，默认禁用全部IRQ
    io_apic_init();     // 初始化IO APIC，默认禁用全部GSI
    local_apic_init();  // 初始化Local APIC，默认禁用全部LVT，启用SVR

    // 知道了CPU的数目，复制per-CPU Data
    console_print("Copying per-CPU Data\n");
    percpu_area_init();

    // 知道了CPU的数目，准备真正的GDT
    console_print("Initializing GDT and TSS\n");
    gdt_init();
    tss_init();

    // 初始化物理内存分配器（最后一个使用kernel_end_addr）
    console_print("Creating buddy bitmap\n");
    page_alloc_init(mbi->mmap_addr, mbi->mmap_length);

    // 初始化并启用PIT时钟
    console_print("Initializing PIT\n");
    pit_init();
    pit_map_gsi(GSI_VEC_BASE + 2);      // TODO: 这里不要硬编码，根据MADT内容映射
    io_apic_unmask(GSI_VEC_BASE + 2);

    __asm__ __volatile__("sti");

    console_print("Initializing timer\n");
    local_apic_timer_init();
    io_apic_mask(GSI_VEC_BASE + 2);

    console_print("Waking up all processors\n");
    // local_apic_start_ap();

    // 安装syscall的ISR
    syscall_init();
    // idt_set_int_handler(80, syscall_dispatcher);

    // 向run_queue中添加进程，当下一次local APIC timer中断触发时，就会自动切换到这些进程中开始执行
    create_process(process_A);
    create_process(process_B);
    create_process(process_C);
    create_process3(ring3);
    create_process3(ring32);

    start_schedule();

    while (true) { }
}

// static spinlock_t lock = 0;

// AP的初始化函数，由trampoline.asm调用
void ap_init(int id) {
    // 设置GS.BASE寄存器，在读取per-CPU Data时需要
    write_gsbase(id);

    char *video = (char*) (KERNEL_VMA + 0xa0000);
    video[160*id + 0] = 'A';
    video[160*id + 1] = 0x0b;
    video[160*id + 2] = 'P';
    video[160*id + 3] = 0x0b;
    
    idt_load();
    local_apic_init();
    gdt_load();
    idt_load();

    char str[] = "X";
    for (int i = 0; i < 1000; ++i) {
        raw_spin_lock(&lock);
            str[0] = 'A'+id;
            console_set_attr(0x0a + id);
            console_print(str);
            console_set_attr(0x1f);
        raw_spin_unlock(&lock);
        pit_delay(8000);
    }

    while (true) { }
}
