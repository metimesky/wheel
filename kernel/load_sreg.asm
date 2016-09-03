global load_sreg

[section .init]
[BITS 64]

load_sreg:
    mov     ax, 16
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    ret
