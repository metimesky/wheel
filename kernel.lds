/* Wheel内核镜像的内存布局 */
/* 文件中定义的所有标记符号都是物理地址 */

/* OUTPUT_ARCH(i386:x86-64); */
OUTPUT_FORMAT(elf64-x86-64);
ENTRY(multiboot_entry);

SECTIONS {
    KERNEL_LMA = 0x100000;
    KERNEL_VMA = 0xffff800000000000;

    . = KERNEL_LMA;

    .boot : AT(ADDR(.boot)) {
        *(.boot)
    } = 0x90

    /* 启动AP的实模式代码，将被复制到0x7c000处执行 */
    __trampoline_start = ALIGN(16);
    .trampoline 0x7c000 : AT(__trampoline_start) {
        *(.trampoline)
    } = 0x90
    __trampoline_end = __trampoline_start + SIZEOF(.trampoline);

    /* 进入higher-half，跳过boot和trampoline的部分 */
    . = KERNEL_VMA + __trampoline_end;

    /* 初始化代码数据段，这部分内存会在初始化完成之后由系统回收 */
    __init_start = . - KERNEL_VMA;
    .init ALIGN(16) : AT(ADDR(.init) - KERNEL_VMA) {
        *(.init)
    } = 0x90
    __init_end = . - KERNEL_VMA;

    /* 内核代码和数据 */
    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VMA) {
        *(.text)
    }
    .rodata ALIGN(16) : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata)
    }
    .data ALIGN(16) : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)
    }

    /* 内核BSS段，无需清零 */
    .bss ALIGN(0x1000) (NOLOAD) : AT(ADDR(.bss) - KERNEL_VMA) {
        /* 初始页表需要在高地址和低地址中访问，故导出两个符号 */
        __early_pgtbl = .;
        __early_pgtbl_low = __early_pgtbl - KERNEL_VMA;
        . += 0x6000;

        *(COMMON)
        *(.bss)
    }

    . = ALIGN(16);
    
    __percpu_start = . - KERNEL_VMA;
    .percpu.data : AT(ADDR(.percpu.data) - KERNEL_VMA) {
        *(.percpu.data)
        kernel_stack = .;
        . += 0x1000;
        kernel_stack_top = .;
        *(.percpu.bss)
    }
    __percpu_end = . - KERNEL_VMA;

    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.note.gnu.build-id)
        *(.eh_frame)
    }
}
