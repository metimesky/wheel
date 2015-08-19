; assembly routines for port io

[section text]
[BITS 64]
global in_byte
in_byte:
    mov     rdx, rdi
    in      al, dx
    ret

global in_word
in_word:
    mov     rdx, rdi
    in      ax, dx
    ret

global in_dword
in_dword:
    mov     rdx, rdi
    in      eax, dx
    ret

global out_byte
out_byte:
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, al
    ret

global out_word
out_word:
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, ax
    ret

global out_dword
out_dword:
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, eax
    ret

global io_wait
io_wait:
    xor     rax, rax
    out     0x80, al
    ret