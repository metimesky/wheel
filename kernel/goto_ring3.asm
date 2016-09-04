extern ring3
global goto_ring3

[section .text]
[BITS 64]

goto_ring3:
    ;mov     rbx, rsp
    mov     rax, qword ring3
    push    32+3    ; ss
    push    rdi     ; rsp
    pushf           ; rflags
    pop     rbx
    or      rbx, 3<<12
    push    rbx
    popf
    pushf
    push    24+3    ; cs
    push    rax     ; rip
    iretq
