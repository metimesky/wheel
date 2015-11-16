; some string function implemented in assembly
; rdi, rsi, rdx, rcx, r8, r9

global fill_with_bytes
;global fill_with_word
;global fill_with_dword
;global fill_with_qword

[section .text]
[BITS 64]
fill_with_bytes:
    ret