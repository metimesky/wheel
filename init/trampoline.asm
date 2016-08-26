[section .init]
[BITS 16]

global trampoline

trampoline:

cli
mov     ax, 0xa000
mov     ds, ax
mov     byte [ds:0], 'X'
mov     byte [ds:1], 0x4e
jmp     $