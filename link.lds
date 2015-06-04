/* linker script for Wheel Operating System */

OUTPUT_FORMAT(binary)

SECTIONS {
    . = 1M;
    kernel_load_addr = .;

    .text BLOCK(4K) : ALIGN(4K) {
        kernel_text_start = .;
        *(.boot)
        *(.text)
        kernel_text_end = .;
    } = 0x90    /* NOP */

    .data BLOCK(4K) : ALIGN(4K) {
        kernel_data_start = .;
        *(.rodata)
        *(.data)
        kernel_data_end = .;
    } = 0

    .bss BLOCK(4K) : ALIGN(4K) {
        kernel_bss_start = .;
        *(COMMON)
        *(.bss)
        kernel_bss_end = .;
    } = 0
}
