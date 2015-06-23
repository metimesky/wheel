;; under x64, parameters are passed by rdi, rsi, rdx, rcx, r8, r9, ...

global load_idtr

[section text]
[BITS 64]
load_idtr:
    lidt    [rdi]
    ret