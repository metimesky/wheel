global goto_ring3
global delay
global ap
global ap_end

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
    pushf               ; rflags, IF=1
    mov     rax, 0x1b
    push    rax         ; cs = user code segment
    push    rdi         ; rip
    iretq

delay:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ret

[section .text]
    ;org 0x7c000
[BITS 16]
ap:
    xor     ax, ax
    mov     gs, ax
    mov     al, 'A'
    mov     ah, 0x0e
    mov     word [gs: 0xb8006], ax
    jmp     $
ap_end: