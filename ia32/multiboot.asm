extern  kernel_load_addr
extern  kernel_data_end
extern  kernel_bss_end

extern  gdt_ptr
extern  idt_ptr
extern  setup_gdt
extern  setup_idt
extern  setup_tss

extern  kmain

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

    ; disable paging to be safe
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax

    ; save magic number and multiboot info
    mov     [mb_magic], eax
    mov     [mb_info], ebx
    
    ; setup kernel stack poiner
    mov     esp, kstack_top

    ; clear eflags register
    push    0
    popf

    ; set frame base register to NULL
    mov     ebp, 0

    ; setup new GDT
    call    setup_gdt
    lgdt    [gdt_ptr]
    jmp     8:.flush_gdt
.flush_gdt:
    ; set all segment register to kernel data segment descriptor
    mov     eax, 16
    mov     ds, eax
    mov     es, eax
    mov     fs, eax
    mov     gs, eax

    ; setup IDT
    call    setup_idt
    lidt    [idt_ptr]

    ;sti
    int     0x40
    jmp     $

    ; setup TSS
    call    setup_tss
    mov     ax, 0x28
    ltr     ax

    ; clear the screen
    mov     edi, 0xb8000
    mov     eax, 0x0f200f20
    mov     ecx, 1000
    rep     stosd

    call    kmain

    ; should never return here
    cli
    hlt
    jmp     $

[SECTION    .data]

mb_magic    dd      0
mb_info     dd      0

[SECTION    .bss]

kstack:     resb    0x1000
kstack_top: