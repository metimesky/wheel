extern ring3
global goto_ring3

[section .text]
[BITS 64]

goto_ring3:
    mov     rbx, rsp
    mov     rax, qword ring3
    push    32+3    ; ss
    push    rbx     ; rsp
    pushf           ; rflags
    push    24+3    ; cs
    push    rax     ; rip
    iretq