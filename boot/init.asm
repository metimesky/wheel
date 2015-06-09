; This is the first piece of code to be executed after the booting stage.
; These code is responsible for detecting and setting up long mode,
; new GDT, and initial PAE paging. Then it jumps into `stage1.c`.

global wheel_init
global kernel_stack
global pml4t

extern wheel_main

[section .text]
[BITS 32]
wheel_init:
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

    ; setup Page-Map Level-4 (PML4) Table, only 1 entry
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; setup Page Directory Pointer (PDP) Table, only 1 entry
    mov     dword [edi], 0x2003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 0x1000

    ; setup Page Directory (PD) Table, only 1 entry
    mov     dword [edi], 0x3003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+12K (Page Table)
    add     edi, 0x1000

    ; setup Page Table, 512 entries
    mov     ebx, 0x00000003         ; present, read/write
    mov     ecx, 512
.set_entry:
    mov     dword [edi], ebx
    add     ebx, 0x1000
    add     edi, 8
    loop    .set_entry

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
    lgdt    [gdt.ptr]

    ; jump to real 64-bit code
    jmp     gdt.code0:long_mode_entry

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
    mov     ax, gdt.data0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    ; Clear the screen
    mov     edi, 0xb8000
    mov     rax, 0x0f200f200f200f20
    mov     ecx, 500
    rep     stosq

    xor     rdi, rdi
    xor     rsi, rsi
    mov     edi, [mb_eax]   ; zero extend to rdi
    mov     esi, [mb_ebx]   ; zero extend to rsi
    call    wheel_main
    sub     rsp, 16

    ; halt on return
    hlt
    jmp     $

[section .data]
[BITS 32]
gdt:                        ; Global Descriptor Table (64-bit).
.null:  equ     $ - gdt     ; null descriptor
    dq      0
.code0: equ     $ - gdt     ; code descriptor
    dw      0               ; -- limit [0:15]
    dw      0               ; -- base [0:15]
    db      0               ; -- base [16:23]
    db      10011000b       ; -- Present, DPL=0, non-conforming
    db      00100000b       ; -- 64-bit
    db      0               ; -- base [24:31]
.data0: equ     $ - gdt     ; data descriptor
    dw      0               ; -- limit [0:15]
    dw      0               ; -- base [0:15]
    db      0               ; -- base [16:23]
    db      10010000b       ; -- Present, DPL=0
    db      0               ; -- limit [16:19] and attr
    db      0               ; -- base [24:31]
.code3: equ     $ - gdt     ; code descriptor
    dw      0               ; -- limit [0:15]
    dw      0               ; -- base [0:15]
    db      0               ; -- base [16:23]
    db      11111000b       ; -- Present, DPL=0, non-conforming
    db      00100000b       ; -- 64-bit
    db      0               ; -- base [24:31]
.data3: equ     $ - gdt     ; data descriptor
    dw      0               ; -- limit [0:15]
    dw      0               ; -- base [0:15]
    db      0               ; -- base [16:23]
    db      11110000b       ; -- Present, DPL=0
    db      0               ; -- limit [16:19] and attr
    db      0               ; -- base [24:31]

.ptr:                       ; The GDT-pointer.
    dw      $ - gdt - 1     ; -- Limit.
    dq      gdt             ; -- Base.

mb_eax:
    dd      0
mb_ebx:
    dd      0

[section .bss]
; reserve 4KB for kernel stack.
kernel_stack:   resb 0x1000
kernel_stack_top:

; reserve 16KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x4000