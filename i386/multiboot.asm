extern  kernel_load_addr
extern  kernel_data_end
extern  kernel_bss_end

extern  init
extern  main

MB1_MAGIC   equ 0x1badb002
MB1_FLAGS   equ 1<<0|1<<1|1<<16
MB1_CHECK   equ -(MB1_MAGIC+MB1_FLAGS)

[SECTION    .boot]
[BITS       32]

    jmp     multiboot_entry
ALIGN   4
multiboot_header:
    dd      MB1_MAGIC
    dd      MB1_FLAGS
    dd      MB1_CHECK
    dd      multiboot_header
    dd      kernel_load_addr
    dd      kernel_data_end
    dd      kernel_bss_end
    dd      multiboot_entry

multiboot_entry:
    ; disable interruption to be safe
    cli

    ; save multiboot magic and info
    mov     [mb_eax], eax
    mov     [mb_ebx], ebx

    ; disable paging for safe
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax
    
    ; setup kernel stack poiner
    mov     esp, kernel_stack_top

    ; clear eflags register
    push    0
    popf

    ; set frame base register to NULL
    mov     ebp, 0

    ; clear the screen
    mov     edi, 0xb8000
    mov     eax, 0x0f200f20
    mov     ecx, 1000
    rep     stosd

    ; pass the parameters
    push    dword [mb_ebx]
    push    dword [mb_eax]
    call    init
    sub     esp, 8

    ;sti
    call    main

    ; should never return here
    cli
    hlt
    jmp     $


[SECTION    .data]

mb_eax      dd  0
mb_ebx      dd  0


[SECTION    .bss]

kernel_stack:   resb    0x1000
kernel_stack_top:

global  kernel_stack_top