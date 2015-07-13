;; under x64, parameters are passed by rdi, rsi, rdx, rcx, r8, r9, ...

global load_idtr
global load_tr

global in_byte
global in_word
global in_dword
global out_byte
global out_word
global out_dword

global io_wait

global cpu_id

[section text]
[BITS 64]
load_idtr:
    lidt    [rdi]
    ret

load_tr:
    mov     rax, rdi
    ltr     ax
    ret

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
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, al
    ret

out_word:
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, ax
    ret

out_dword:
    mov     rdx, rdi
    mov     rax, rsi
    out     dx, eax
    ret

io_wait:
    xor     rax, rax
    out     0x80, al
    ret

; void cpu_id(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
cpu_id:
    push    rbx         ; first save rbx
    mov     r8, rdx     ; mov the value of 2nd, 3rd params to other
    mov     r9, rcx     ; registers, since cpuid write value to rdx and rcx.
    mov     eax, [rdi]
    cpuid
    mov     [rdi], eax
    mov     [rsi], ebx
    mov     [r8], ecx
    mov     [r9], edx
    pop     rbx         ; restore rbx