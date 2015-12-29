; this file contains the implementation of several Clib functions
; written in assembly to maximize performance.

; System V ABI, arguments:
; rdi, rsi, rdx, rcx, r8, r9

;global strcpy

[SECTION .text]
; char *strcpy(char *dst, const char *src);
; rdi is already dst string and rsi is already source string, perfect!
asm_strcpy:
    mov     rbx, rdi    ; save dst parameter
    lodsb
    stosb
    test    al, al
    jne     asm_strcpy
    mov     rax, rbx
    ret

; char *strncpy(char *dst, const char *src, size_t n);
strncpy:

; size_t strlen(const char *s);
asm_strlen:
    mov     rcx, -1
    xor     al, al
    repne scasb
    mov     rax, -2
    sub     rax, rcx
    ret

; void *memcpy(void *dst, const char *src, size_t n);
asm_memcpy:
    mov     rax, rdi    ; save dst for return value
    mov     rcx, rdx    ; bytes count
    shr     rcx, 3      ; now rcx contains number of qwords
    and     rdx, 7      ; now rdx contains number of remaining
    rep movsq
    mov     rcx, rdx
    rep movsb
    ret

; void *memset(void *dst, unsigned char ch, size_t n);
asm_memset:
    ; expand byte to qword
    and     rsi, 0x00000000000000ff
    mov     rax, 0x0101010101010101
    imul    rax, rsi

    ; save return value
    mov     r8, rdi

    mov     rcx, rdx    ; bytes count
    shr     rcx, 3      ; now rcx contains the number of qwords
    and     rdx, 7      ; now rdx is the remaining bytes

    rep stosq
    mov     rcx, rdx
    rep stosb
    
    mov     rax, r8
    ret

; int memcmp(const void *s1, const void *s2, size_t n);
asm_memcmp:
    mov     rcx, rdx    ; bytes count
    shr     rcx, 3      ; now rcx contains the number of qwords
    and     rdx, 7      ; now rdx is the remaining bytes