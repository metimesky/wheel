/* Wheel内核镜像的内存布局 */
/* 文件中定义的所有标记符号都是物理地址 */

OUTPUT_ARCH(i386:x86-64);
ENTRY(multiboot_entry);

SECTIONS {
    LOAD_OFFSET = 0x100000;             /* 加载的物理地址 */
    KERNEL_VMA = 0xffff800000000000;    /* 内核起始的虚拟地址 */

    . = LOAD_OFFSET;

    .boot : AT(ADDR(.boot)) {
        *(.boot)
    } = 0x90

    /* 启动AP的实模式代码，将被复制到0x7c000处执行 */
    trampoline_start_addr = ALIGN(16);
    .trampoline 0x7c000 : AT(trampoline_start_addr) {
        *(.trampoline)
    } = 0x90
    trampoline_end_addr = trampoline_start_addr + SIZEOF(.trampoline);

    /* 进入higher-half，跳过boot和trampoline的部分 */
    . = KERNEL_VMA + trampoline_end_addr;

    .init ALIGN(16) : AT(ADDR(.init) - KERNEL_VMA) {
        *(.init)
    } = 0x90

    init_end_addr = . - KERNEL_VMA;

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VMA) {
        *(.text)
    }

    .rodata ALIGN(16) : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata)
    }

    .data ALIGN(16) : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)

        *(.symbols)
        PROVIDE(kernel_symbols_start = .);
        PROVIDE(kernel_symbols_end = .);
        PROVIDE(bss_start = .);
    }

    .bss ALIGN(0x1000) (NOLOAD) : AT(ADDR(.bss) - KERNEL_VMA) {
        /* 初始页表需要在高地址和低地址中访问，故导出两个符号 */
        initial_pml4t = .;
        initial_pml4t_low = initial_pml4t - KERNEL_VMA;
        . += 0x6000;

        *(COMMON)
        *(.bss)
    }

    . = ALIGN(16);
    
    percpu_start_addr = . - KERNEL_VMA;
    .percpu.data : AT(ADDR(.percpu.data) - KERNEL_VMA) {
        *(.percpu.data)
    }
    percpu_data_end_addr = . - KERNEL_VMA;
    .percpu.bss (NOLOAD) : AT(ADDR(.percpu.bss) - KERNEL_VMA) {
        kernel_stack = .;
        . += 0x1000;
        kernel_stack_top = .;
        *(.percpu.bss)
    }
    percpu_end_addr = . - KERNEL_VMA;

    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.note.gnu.build-id)
        *(.eh_frame)
    }
}
