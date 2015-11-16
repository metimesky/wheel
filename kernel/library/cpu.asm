; first function argument is passed using rdi
; second argument is passed using rsi

; It is better to implement all functions as inline assembly
; But currently I can't figure out how

global load_gdtr
global load_idtr
global load_tr

[section .text]
[BITS 32]
load_gdtr:
    lgdt    [edi]
    jmp     0x08:flush_gdt
    jmp     $
[BITS 64]
flush_gdt:
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret

load_idtr:
    lidt    [rdi]
    ret

load_tr:
    mov     rax, rdi
    ltr     ax
    ret