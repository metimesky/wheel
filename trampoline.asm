; this file should not be compiled to kernel

[BITS 16]

org 0x7c000

mov     ax, 0xb800
mov     gs, ax
mov     al, 'X'
mov     ah, 0x1e
mov     [gs:6], ax
jmp     $
jmp     $
