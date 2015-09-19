; This is the entry point of kernel, the control is handed over from GRUB.
; In this file, 

extern  kernel_start
extern  kernel_data_end
extern  kernel_bss_end

extern  kernel_main

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
    dd      kernel_start        ; load_addr
    dd      kernel_data_end     ; load_end_addr
    dd      kernel_bss_end      ; bss_end_addr
    dd      multiboot_entry     ; entry_addr

multiboot_entry:
    ; disable interrupt
    cli

    ; store boot information
    mov     [mb_eax], eax
    mov     [mb_ebx], ebx

    ; setup kernel stack
    mov     esp, kernel_stack_top

    ; clear ebp and eflags
    mov     ebp, 0
    push    0
    popf

    ; check whether CPUID is supported by setting bit 21 of eflags
    pushfd
    pop     eax
    mov     ecx, eax
    xor     eax, 1 << 21
    push    eax
    popfd
    pushfd
    pop     eax
    push    ecx
    popfd
    xor     eax, ecx
    jz      no_long_mode    ; CPUID not supported, thus no long mode
    
    ; check CPUID extended functions
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    jb      no_long_mode    ; CPUID extended functions not available, thus no long mode
    
    ; check long mode existence
    mov     eax, 0x80000001
    cpuid
    test    edx, 1 << 29
    jz      no_long_mode

enter_long_mode:
    ; disable paging (to be safe)
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax

    ; clear 16KB of memory for 4 page tables, which covers 2MB address space.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 4096
    rep     stosd
    mov     edi, cr3

    ; filling one Page-Map Level-4 Entry (PML4E)
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; setup Page Directory Pointer (PDPE) Table, only 1 entry
    mov     dword [edi], 0x2003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 0x1000

    ; setup Page Directory (PDE) Table, 512 entry
    mov     ebx, 0x00000083         ; present, read/write, 1G granularity
    mov     ecx, 512                ; 512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 21
    add     edi, 8
    loop    .set_pdp_entry

    ; enable PAE-paging
    mov     eax, cr4
    or      eax, 1 << 5
    mov     cr4, eax

    ; set LM-bit
    mov     ecx, 0xc0000080
    rdmsr
    or      eax, 1 << 8
    wrmsr

    ; enable paging
    mov     eax, cr0
    or      eax, 1 << 31
    mov     cr0, eax

    ; enter 64-bit submode
    lgdt    [tmp_gdt_ptr]

    ; jump to real 64-bit code
    jmp     tmp_gdt.code:long_mode_entry

    ; should never return here
    hlt
    jmp     $

no_long_mode:
    ; write error message
    mov     edi, 0xb8000
    mov     esi, .err_msg
    mov     ecx, .err_msg_len
    mov     ah, 0x4e        ; yellow on red
.print:
    lodsb
    stosw
    loop    .print

    ; halt the machine
    hlt
    jmp     $

.err_msg        db      "CPU is not compatible with 64-bit mode."
.err_msg_len    equ     $ - .err_msg

[BITS 64]
long_mode_entry:
    mov     ax, tmp_gdt.data
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    ; clear the screen
    mov     edi, 0xb8000
    mov     rax, 0x0f200f200f200f20
    mov     ecx, 500
    rep     stosq

    mov     edi, [mb_eax]
    mov     esi, [mb_ebx]
    call    kernel_main

    ; halt on return
    hlt
    jmp     $

[section .data]
[BITS 32]
; temporary GDT (64-bit).
tmp_gdt:
.null   equ $ - tmp_gdt
    dq      0
.code   equ $ - tmp_gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10011000b       ; Present, DPL=0, non-conforming
    db      10100000b       ; 64-bit, and limit[16:19]
    db      0               ; base [24:31]
.data   equ $ - tmp_gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10010010b       ; Present, DPL=0, writable
    db      11000000b       ; limit [16:19] and attr
    db      0               ; base [24:31]

tmp_gdt_ptr:
    dw      $ - tmp_gdt - 1     ; limit
    dq      tmp_gdt             ; base

; temporary store the multiboot info
mb_eax: dd  0
mb_ebx: dd  0

[section .bss]
; reserve 4KB for kernel stack.
kernel_stack:   resb 0x1000
kernel_stack_top:

; reserve 16KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x4000

ring3_stack:    resb 0x1000
ring3_stack_top: