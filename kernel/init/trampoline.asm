; this file is the trampoline code of AP startup.
; AP starts in real mode, so we have to switch to protected mode, then
; switch to long mode.
; When switching to long mode, we can make use of the original code.

global trampoline
global trampoline_end

[section .text]

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

    jmp     $

    ; AP is different with BSP, we don't need to enable A20
    mov     eax, real_trampoline
    push    0
;    jmp far real_trampoline     ; use far jump, avoid relative addressing

    jmp     $
    jmp     $
trampoline_end:


real_trampoline:
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
