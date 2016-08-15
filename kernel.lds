OUTPUT_ARCH(i386:x86-64);

ENTRY(kernel_entry);

SECTIONS {
    LOAD_OFFSET = 0xffff800000000000;

    . = 0x100000;

    .boot ALIGN(0x1000) : AT(ADDR(.boot)) {
        *(.boot)
    } = 0x90

    .boot.bss ALIGN(0x1000) (NOLOAD) : AT(ADDR(.boot.bss)) {
        *(.boot.bss)
    } = 0

    . += LOAD_OFFSET;

    .init ALIGN(0x1000) : AT(ADDR(.init) - LOAD_OFFSET) {
        *(.init)
    } = 0x90

    .text ALIGN(0x1000) : AT(ADDR(.text) - LOAD_OFFSET) {
        *(.text)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - LOAD_OFFSET) {
        *(.rodata)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - LOAD_OFFSET) {
        *(.data)
    }

    .bss ALIGN (0x1000) (NOLOAD) : AT(ADDR(.bss) - LOAD_OFFSET) {
        *(COMMON)
        *(.bss)
    }
    
    kernel_end_addr = .;

    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }
}
