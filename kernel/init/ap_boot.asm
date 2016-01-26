; this file is the trampoline code of AP startup.
; AP starts in real mode, so we have to switch to protected mode, then
; switch to long mode.
; When switching to long mode, we can make use of the original code.

; Since this piece of code is compiled as 16-bit, when relocating, the
; 64-bit virtual address cannot be fit in 16-bit slot.

extern gdt
extern gdt_ptr
extern pml4t
extern ap_post_init

; To avoid relocation truncation, we have to calculate effective address by
; hard coding target virtual address 0x7c000.
%define addr(v) (0x7c000 + v - $$)

[section .ap_boot]
[BITS 16]
ap_entry:
    ; disable interrupt
    cli

    ; init segment registers
    mov     ax, 0x7c00
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     fs, ax
    mov     ax, 0xb800
    mov     gs, ax

    ; AP is different with BSP, we don't need to enable A20
    lgdt    [tmp_gdt_ptr-$$]     ; it will truncated to 16-bit

    ; enable protected mode
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    ; jump into 32-bit protected mode code
    jmp     dword tmp_gdt.code:addr(start32)

    hlt
    jmp     $

; 32-bit protected mode GDT
ALIGN 8
tmp_gdt:
.null   equ $ - tmp_gdt
    dd      0
    dd      0
.code   equ $ - tmp_gdt
    dw      0xffff          ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10011000b       ; Present, DPL=0, Execute/Read, non-conforming
    db      11001111b       ; Granularity, 32-bit, limit[16:19]
    db      0               ; base [24:31]
.data   equ $ - tmp_gdt
    dw      0xffff          ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10010010b       ; Present, DPL=0, Read/Write
    db      11001111b       ; limit [16:19] and attr
    db      0               ; base [24:31]

tmp_gdt_ptr:
    dw      $ - tmp_gdt - 1 ; limit
    dd      addr(tmp_gdt)   ; base


[BITS 32]
start32:
    mov     ax, tmp_gdt.data
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    ; set page table address
    mov     edi, pml4t
    mov     cr3, edi

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
    lgdt    [gdt_ptr]
    jmp     8:start64

    hlt
    jmp     $

[BITS 64]
start64:
    ; init segment registers
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    ; init fs and gs (can be used as thread local storage)
    xor     rax, rax
    mov     ecx, 0xc0000100     ; FS.base
    wrmsr
    mov     ecx, 0xc0000101     ; GS.base
    wrmsr

    ; read the stack top address from 0x7cff8
    mov     rax, [0x7cff8]
    mov     rsp, rax

    call    ap_post_init

    ; should never return
    hlt
    jmp     $