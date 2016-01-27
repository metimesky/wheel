/* linker script for Wheel Operating System */

/*OUTPUT_FORMAT(binary)*/
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }

    kernel_text_start = 16M;
    .text kernel_text_start : AT(kernel_text_start) {
        *(.boot)
        *(.text*)
    } = 0x90
    kernel_text_end = kernel_text_start + SIZEOF(.text);

    ap_boot_start = ALIGN(kernel_text_end, 16);
    .ap_boot 0x7c000 : AT(ap_boot_start) {
        *(.ap_boot)
    } = 0x90
    ap_boot_end = ap_boot_start + SIZEOF(.ap_boot);

    kernel_data_start = ALIGN(ap_boot_end, 16);
    .data kernel_data_start : AT(kernel_data_start) {
        *(.rodata*)
        *(.data*)
    } = 0
    kernel_data_end = kernel_data_start + SIZEOF(.data);

    kernel_bss_start = ALIGN(kernel_data_end, 16);
    .bss kernel_bss_start : AT(kernel_bss_start) {
        *(COMMON)
        *(.bss)
    } = 0
    kernel_bss_end = kernel_bss_start + SIZEOF(.bss);
}
