/* linker script for Wheel Operating System */

/*OUTPUT_FORMAT(binary)*/
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }

    . = 0x7c000;
    .tramp : {
        *(.tramp)
        trampoline_end = .;
    }

    . = 1M;
    kernel_start = .;

    .text /* BLOCK(4K) */ : /* ALIGN(4K) */ {
        kernel_text_start = .;
        *(.boot)
        /*
        *(.tramp)
        trampoline_end = .;
        */
        *(.text*)
        kernel_text_end = .;
    } = 0x90    /* NOP */

    .data /* BLOCK(4K) */ : AT(ADDR(.text)+SIZEOF(.text)) /* ALIGN(4K) */ {
        kernel_data_start = .;
        *(.rodata*)
        *(.data*)
        kernel_data_end = .;
    } = 0

    .bss /* BLOCK(4K) */ : AT(ADDR(.data)+SIZEOF(.data)) /* ALIGN(4K) */ {
        kernel_bss_start = .;
        *(COMMON)
        *(.bss)
        kernel_bss_end = .;
    } = 0

    kernel_end = .;
}
