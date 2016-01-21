; this file is the trampoline code of AP startup.
; AP starts in real mode, so we have to switch to protected mode, then
; switch to long mode.
; When switching to long mode, we can make use of the original code.

global trampoline

[section .tramp]

[BITS 16]
trampoline:
    mov     ax, 0xb800
    mov     gs, ax
    mov     ah, 0x0e
    mov     al, 'H'
    mov     [gs:10], ax
    mov     al, 'e'
    mov     [gs:12], ax
    mov     al, 'l'
    mov     [gs:14], ax
    mov     al, 'l'
    mov     [gs:16], ax
    mov     al, 'o'
    mov     [gs:18], ax

    ; AP is different with BSP, we don't need to enable A20
    ;lgdt    [tmp_gdt_ptr]
    cli
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax
    ;jmp     tmp_gdt.code:pm_trampoline

    jmp     $

tmp_gdt:
.null   equ $ - tmp_gdt
    dd      0
    dd      0
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
    dw      $ - tmp_gdt - 1 ; limit
    dd      tmp_gdt         ; base

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
