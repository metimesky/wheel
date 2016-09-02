extern ring3
global goto_ring3

[section .text]
[BITS 64]

goto_ring3:
    mov     rax, qword ring3
    push    rax
    push    24+3
    push    0
    push    0x1000
    push    32+3
    iretq