global sys_print
global sys_gettick

[section .text]
[BITS 64]

sys_print:
    mov     rax, 0
    int     80
    ret

sys_gettick:
    mov     rax, 1
    int     80
    ret