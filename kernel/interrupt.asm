;; this file is the entry point of an interrupt.

global interrupt_entry

; C: void handler(uint32_t errcode, uint64_t rip, uint64_t rflags, uint64_t rsp);
extern handler

[section .text]
[BITS 64]
interrupt_entry:
    ; if has err code, pop it from the stack
    ; under x64, parameters are passed by rdi, rsi, rdx, rcx, r8, r9, ...
    pop     rdi     ; if no errcode, mov 0xffffffffffffffff to rdi
    ; we can't use pop now, since iretd need this information.
    mov     rsi, [rsp]      ; return rip
    mov     rdx, [rsp+16]   ; return rflags
    mov     rcx, [rsp+24]   ; return rsp
    call    [handler+8*i]
