#include <wheel.h>
#include <multiboot.h>
#include <interrupt.h>
#include <drivers/console.h>
#include <drivers/acpi/acpi.h>
#include <drivers/pit.h>

// 多核初始化
// GDT、分页、IDT等配置对于每个核心都要执行，因此最好等到所有核心都启动后再初始化

// 中断，需要计划好IDT每个条目的作用

// extern void apic_init();
extern void local_apic_timer_init();
extern void local_apic_send_ipi();

void init(uint32_t eax, uint32_t ebx) {
    console_init();
    // console_print("Welcome to Wheel OS!\n");

    // 检查magic number
    if (MULTIBOOT_BOOTLOADER_MAGIC != eax) {
        console_print("Multiboot magic not compliant!\n");
        return;
    }

    // early ACPI table access
    if (!initialize_acpi_tables()) {
        console_print("ACPI not available!\n");
        return;
    }

    console_print("Initializing Interrupt\n");
    interrupt_init();

    char *video = (char *)(KERNEL_VMA + 0xa0000);
    video[158] = '0';
    video[156] = '0';

    console_print("Initializing PIT\n");
    pit_init();
    pit_map_gsi(GSI_VEC_BASE + 2);
    io_apic_unmask(GSI_VEC_BASE + 2);

    __asm__ __volatile__("sti");

    local_apic_timer_init();

    local_apic_send_ipi();

    while (true) { }
}