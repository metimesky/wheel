; this file is the trampoline code of AP startup.
; AP starts in real mode, so we have to switch to protected mode, then
; switch to long mode.
; When switching to long mode, we can make use of the original code.

; Since this piece of code is compiled as 16-bit, when relocating, the
; 64-bit virtual address cannot be fit in 16-bit slot.

global trampoline

; To avoid relocation truncation, we have to calculate effective address by
; hard coding target virtual address 0x7c000.
%define addr(v) (0x7c000 + v - $$)

[section .tramp]
[BITS 16]

; AP starts executing this code in real mode.
; 16 bit mode real mode is too limited, we have to use full address to
; express them.
trampoline:
    ; disable interrupt
    cli

    ; init segment registers
    mov     ax, 0x7c00
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     ax, 0xb800
    mov     gs, ax

    ; print information
    mov     ah, 0x0e
    mov     al, 'H'
    mov     [gs:10], ax
    mov     al, 'a'
    mov     [gs:12], ax
    mov     al, 'l'
    mov     [gs:14], ax
    mov     al, 'l'
    mov     [gs:16], ax
    mov     al, 'o'
    mov     [gs:18], ax

    ; AP is different with BSP, we don't need to enable A20
    lgdt    [ds:(tmp_gdt_ptr - $$)]     ; it will truncated to 16-bit
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax
    ;jmp     $
    jmp     dword 8:addr(pm_trampoline)

    hlt
    jmp     $

; 32-bit protected mode GDT
tmp_gdt:
.null   equ $ - tmp_gdt
    dd      0
    dd      0
.code   equ $ - tmp_gdt
    dw      0xffff          ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10011010b       ; Present, DPL=0, Execute/Read, non-conforming
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
    dw      23              ; limit
    dd      addr(tmp_gdt)   ; base

[BITS 32]
pm_trampoline:
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    mov     ax, 0xb800
    mov     gs, ax
    mov     ah, 0x0c
    mov     al, 'W'
    mov     [gs:22], ax
    mov     al, 'o'
    mov     [gs:24], ax
    mov     al, 'r'
    mov     [gs:26], ax
    mov     al, 'l'
    mov     [gs:28], ax
    mov     al, 'd'
    mov     [gs:30], ax

    jmp     $
    jmp     $
