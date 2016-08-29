; 这段代码将被复制到0x7c000处，从实模式开始执行

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