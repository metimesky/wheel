OUTPUT_ARCH(i386:x86-64);

ENTRY(kernel_entry);

SECTIONS {
    LOAD_OFFSET = 0xffff800000000000;

    . = 0x100000;

    .boot ALIGN(0x1000) : AT(ADDR(.boot)) {
        *(.boot)
    } = 0x90

    . += LOAD_OFFSET;

    .init ALIGN(0x1000) : AT(ADDR(.init) - LOAD_OFFSET) {
        *(.init)
    } = 0x90

    init_end_addr = . - LOAD_OFFSET;

    .text ALIGN(0x1000) : AT(ADDR(.text) - LOAD_OFFSET) {
        *(.text)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - LOAD_OFFSET) {
        *(.rodata)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - LOAD_OFFSET) {
        *(.data)
    }

    .bss ALIGN(0x1000) (NOLOAD) : AT(ADDR(.bss) - LOAD_OFFSET) {
        /* 初始页表需要在高地址和低地址中访问，故导出两个符号 */
        initial_pml4t = .;
        initial_pml4t_low = initial_pml4t - LOAD_OFFSET;
        . += 0x6000;

        *(COMMON)
        *(.bss)
    }

    percpu_start_addr = . - LOAD_OFFSET;
    .percpu.data ALIGN(0x1000) : AT(ADDR(.percpu.data) - LOAD_OFFSET) {
        *(.percpu.data)
    }
    percpu_data_end_addr = . - LOAD_OFFSET;
    .percpu.bss (NOLOAD) : AT(ADDR(.percpu.bss) - LOAD_OFFSET) {
        *(.percpu.bss)
    }
    percpu_end_addr = . - LOAD_OFFSET;

    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }
}
