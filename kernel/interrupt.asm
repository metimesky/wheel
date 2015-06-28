;; this file is the entry point of an interrupt.

; C: void handler(uint32_t errcode, uint64_t rip, uint64_t rflags, uint64_t rsp);
extern interrupt_handler_table

; macro to define hardware interrupt routine entry
; arg[0] -- vector no; arg[1] -- has errcode
%macro hwexp_template 2
global hwint%1
hwint%1:
    mov     r8, %1
%if %2
    pop     rdi
%else
    mov     rdi, 0xffffffffffffffff
%endif
    ; we can't use pop now, since iretd need this information.
    mov     rsi, [rsp]      ; return rip
    mov     rdx, [rsp+16]   ; return rflags
    mov     rcx, [rsp+24]   ; return rsp
    call    [interrupt_handler_table+8*%1]
    iretd
%endmacro

%macro hwint_template 1
global hwint%1
hwint%1:
    mov     rdi, %1 - 32
    ; we can't use pop now, since iretd need this information.
    mov     rsi, [rsp]      ; return rip
    mov     rdx, [rsp+16]   ; return rflags
    mov     rcx, [rsp+24]   ; return rsp
    call    [interrupt_handler_table+8*%1]
    iretd
%endmacro

[section .text]
[BITS 64]

hwexp_template  0, 0        ; #DE divided-by-zero-error exception, no errcode
hwexp_template  1, 0        ; #DB debug exception, no errcode
hwexp_template  2, 0        ; NMI non-maskable-interrupt exception, no errcode
hwexp_template  3, 0        ; #BP breakpoint exception, no errcode
hwexp_template  4, 0        ; #OF overflow exception, no errcode
hwexp_template  5, 0        ; #BR bound-range exception, no errcode
hwexp_template  6, 0        ; #UD invalid-opcode exception, no errcode
hwexp_template  7, 0        ; #NM device-not-available exception, no errcode
hwexp_template  8, 1        ; #DF double-fault exception, errcode = 0
hwexp_template  9, 0        ; coprocessor-segment-overrun exception, reserved
hwexp_template 10, 1        ; #TS invalid-TSS exception
hwexp_template 11, 1        ; #NP segment-not-present exception
hwexp_template 12, 1        ; #SS stack exception
hwexp_template 13, 1        ; #GP general-protection exception
hwexp_template 14, 1        ; #PF page-fault exception
hwexp_template 15, 0        ; reserved
hwexp_template 16, 0        ; #MF x87 floating-point exception-pending, no errcode
hwexp_template 17, 1        ; #AC alignment-check exception, errcode = 0
hwexp_template 18, 0        ; #MC machine-check exception, no errcode
hwexp_template 19, 0        ; #XF SIMD floating-point exception, no errcode

hwexp_template 30, 1        ; #SX security exception, errcode = 1

; external interrupts
hwint_template 32           ; clock
hwint_template 33           ; clock
hwint_template 34           ; clock
hwint_template 35           ; clock
hwint_template 36           ; clock