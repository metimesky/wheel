extern  kernel_load_addr
extern  kernel_data_end
extern  kernel_bss_end

MB_MAGIC    equ 0x1badb002
MB_FLAGS    equ 1<<0|1<<1|1<<16
MB_CHECK    equ -(MB_MAGIC+MB_FLAGS)

[SECTION    .boot]
[BITS       32]

    jmp     multiboot_entry
ALIGN   4
multiboot_header:
    dd      MB_MAGIC
    dd      MB_FLAGS
    dd      MB_CHECK
    dd      multiboot_header
    dd      kernel_load_addr
    dd      kernel_data_end
    dd      kernel_bss_end
    dd      multiboot_entry

multiboot_entry:
    cli

    mov     al, 'H'
    mov     ah, 0x0f
    mov     [0xb8000], ax

    jmp     $
