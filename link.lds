/* linker script for Wheel Operating System */

OUTPUT_FORMAT(binary)

SECTIONS {
    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }

    . = 0x100000;
    kernel_load_addr = .;

    text_start_addr = .;
    .text /* BLOCK(4K) */ : /* ALIGN(4K) */ {
        kernel_text_start = .;
        *(.boot)
        *(.text*)
        kernel_text_end = .;
    } = 0x90    /* NOP */

    .data /* BLOCK(4K) */ : /* ALIGN(4K) */ {
        kernel_data_start = .;
        *(.rodata*)
        *(.data*)
        kernel_data_end = .;
    } = 0
    data_end_addr = .;

    .bss /* BLOCK(4K) */ : /* ALIGN(4K) */ {
        kernel_bss_start = .;
        *(COMMON)
        *(.bss)
        kernel_bss_end = .;
    } = 0
    bss_end_addr = .;

    kernel_end = .;
}
