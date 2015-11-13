; first function argument is passed using rdi
; second argument is passed using rsi


;global in_byte
;global in_word
;global in_dword

;global out_byte
;global out_word
;global out_dword

;global io_wait
global load_idtr

[section .text]
[BITS 64]
in_byte:
    mov     rdx, rdi
    in      al, dx
    ret

in_word:
    mov     rdx, rdi
    in      ax, dx
    ret

in_dword:
    mov     rdx, rdi
    in      eax, dx
    ret

out_byte:
    mov     rdx, rdi    ; port
    mov     rax, rsi    ; val
    out     dx, al
    ret

out_word:
    mov     rdx, rdi    ; port
    mov     rax, rsi    ; val
    out     dx, ax
    ret

out_dword:
    mov     rdx, rdi    ; port
    mov     rax, rsi    ; val
    out     dx, eax
    ret

io_wait:
    xor     rax, rax
    out     0x80, al
    ret

load_idtr:
    lidt    [rdi]
    ret