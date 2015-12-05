global goto_ring3

; void goto_ring3(void *addr, void *rsp);

; in this function, we setup a fake stack to perform a iret
[section .text]
[BITS 64]
goto_ring3:
    ; align the rsp at 16-byte boundary
    mov     rax, rsp
    and     rax, 0xfffffffffffffff0
    mov     rsp, rax

    mov     rax, 0x23
    push    rax         ; ss = user data segment
    push    rsi         ; rsp
    mov     rax, 0x0100
    push    rax         ; rflags, IF=1
    mov     rax, 0x1b
    push    rax         ; cs = user code segment
    push    rdi         ; rip
    iretq