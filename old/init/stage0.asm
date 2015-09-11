; setup temporary 64-bit environment

extern stage1

[section .text]
[BITS 32]
global stage0
stage0:
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

    ; clear 8KB of memory for 2 page tables
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 2048
    rep     stosd
    mov     edi, cr3

    ; setup Page-Map Level-4 (PML4) Table, only 1 entry
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; setup Page Directory Pointer (PDP) Table, 512 entries
    mov     ebx, 0x00000083         ; present, read/write, 1G granularity
    mov     ecx, 512                ; 512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 0x1000
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
    call    stage1

    ; halt on return
    ;ud2
    ;sti
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

; reserve 8KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x2000

ring3_stack:    resb 0x1000
ring3_stack_top: