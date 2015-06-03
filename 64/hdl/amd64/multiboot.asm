MB1_MAGIC   equ 0x1badb002
MB1_FLAGS   equ 1<<0|1<<1|1<<16
MB1_CHECK   equ -(MB1_MAGIC+MB1_FLAGS)

[section .boot]
[BITS 32]
    jmp     multiboot_entry
ALIGN 4
multiboot_header:
    dd      MB1_MAGIC
    dd      MB1_FLAGS
    dd      MB1_CHECK
    dd      multiboot_header
    dd      1
    dd      1
    dd      1
    dd      multiboot_entry

multiboot_entry:
    ; disable interrupt
    cli
