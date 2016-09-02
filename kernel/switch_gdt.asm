global switch_gdt

[section .init]
[BITS 64]

switch_gdt:
    mov     ax, 16
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    ret
