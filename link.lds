/* linker script for Wheel Operating System */

/*OUTPUT_FORMAT(binary)*/
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
        *(.eh_frame)
    }

    kernel_start = 16M;

    kernel_text_start = kernel_start;
    .text kernel_text_start : AT(kernel_text_start) {
        *(.boot)
        *(.text*)
    } = 0x90
    kernel_text_end = kernel_text_start + SIZEOF(.text);

    trampoline_start = ALIGN(kernel_text_end, 16);
    .tramp 0x7c000 : AT(trampoline_start) {
        *(.tramp)
    } = 0x90
    trampoline_end = trampoline_start + SIZEOF(.tramp);

    kernel_data_start = ALIGN(trampoline_end, 16);
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

    kernel_end = .;
}
