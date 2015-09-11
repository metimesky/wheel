; void load_gdtr(gdtr_t *)

[section .text]
[BITS 64]
global load_gdtr
load_gdtr:
    lgdt    [rdi]
    ret