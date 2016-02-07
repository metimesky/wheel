global reload_gdtr

[section .text]
[BITS 64]
reload_gdtr:
    lgdt    [rdi]
    jmp     8:.flush

    hlt
    jmp     $

.flush:
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    ret