extern  kernel_load_addr
extern  kernel_data_end
extern  kernel_bss_end

MB1_MAGIC   equ 0x1badb002              ; magic number
MB1_FLAGS   equ 1<<0|1<<1|1<<16         ; aligned, mem info, address info valid
MB1_CHECK   equ -(MB1_MAGIC+MB1_FLAGS)

[section .boot]
[BITS 32]
    jmp     multiboot_entry
ALIGN 4
multiboot_header:
    dd      MB1_MAGIC
    dd      MB1_FLAGS
    dd      MB1_CHECK
    dd      multiboot_header    ; header_addr
    dd      kernel_load_addr    ; load_addr
    dd      kernel_data_end     ; load_end_addr
    dd      kernel_bss_end      ; bss_end_addr
    dd      multiboot_entry     ; entry_addr

multiboot_entry:
    ; disable interrupt
    cli

    mov     al, 'F'
    mov     ah, 0x0e
    mov     [0xb8000], ax

    jmp     $

;[section .data]
;    mb_eax  dd  0