extern ring3
extern ring32
global goto_ring3
global goto_ring32

[section .text]
[BITS 64]

goto_ring3:
    ;mov     rbx, rsp
    push    32+3    ; ss
    push    rdi     ; rsp
    pushf           ; rflags
    pop     rbx
    or      rbx, 3<<12
    push    rbx
    popf
    pushf
    push    24+3    ; cs
    mov     rax, qword ring3
    push    rax     ; rip
    iretq

goto_ring32:
    ;mov     rbx, rsp
    push    32+3    ; ss
    push    rdi     ; rsp
    pushf           ; rflags
    pop     rbx
    or      rbx, 3<<12
    push    rbx
    popf
    pushf
    push    24+3    ; cs
    mov     rax, qword ring32
    push    rax     ; rip
    iretq
