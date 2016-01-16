;; this file is the entry point of an interrupt.
; TERMS:
;   ISR: Interrupt service routine, handler for exceptions
;   IRQ: Interrupt Request, handler for hardware interrupts
;-------------------------------------------------------------------------------
; This file does not distinguish hardware interrupts and software exceptions
; the difference is handled in C functions.

; there are different types of interrupt
; - for internal exceptions, they might have err code
; - for external interrupts from PIC, we have to send EOI by writing to port
; - for external interrupts from APIC, we have to send EOI by writing to memory

; no matter which type of interrupt, the numbering is continuous

extern interrupt_handler_table

[BITS 64]

; internal exception with no error code, pushes a dummy error code
%macro define_isr 1
global isr%1
isr%1:
    push    -1              ; sign-extended to 64bit
    common_int_handler %1
%endmacro

; internal exception with an error code
%macro define_isr_e 1
global isr%1
isr%1:
    common_int_handler %1
%endmacro

; save the context, after ss, rsp, rflags, cs, rip
%macro save_regs 0
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rdi
    push    rsi
    push    rbp
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
%endmacro

; restore the context
%macro restore_regs 0
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    pop     rsi
    pop     rdi
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax
%endmacro

; common part of interrupt handlers
%macro common_int_handler 1
    ; check if we are comming from user-mode
    test    word [rsp+24], 3    ; by checking SS selector's RPL
    ;jnz     come_from_userspace

    ; push the rest of the interrupt frame to the stack
    save_regs

    cld

    ; save frame pointer to rbp
    mov    rbp, rsp

    ; Call the interrupt handler.
    mov     rdi, %1
    mov     rsi, rbp
    call    [interrupt_handler_table + 8 * %1]

    ; restore saved frame pointer from rbp
    mov     rsp, rbp

    ; restore the saved registers.
    restore_regs

    ; skip error code
    add     rsp, 8

    iretq
%endmacro

[section .text]
[BITS 64]
; internal exceptions, well-defined
define_isr 0
define_isr 1
define_isr 2
define_isr 3
define_isr 4
define_isr 5
define_isr 6
define_isr 7
define_isr_e 8
define_isr 9
define_isr_e 10
define_isr_e 11
define_isr_e 12
define_isr_e 13
define_isr_e 14
define_isr 15
define_isr 16
define_isr_e 17
define_isr 18
define_isr 19
define_isr 20
define_isr 21
define_isr 22
define_isr 23
define_isr 24
define_isr 25
define_isr 26
define_isr 27
define_isr 28
define_isr 29
define_isr 30
define_isr 31

; other not defined interrupts, for later use
define_isr 32
define_isr 33
define_isr 34
define_isr 35
define_isr 36
define_isr 37
define_isr 38
define_isr 39
define_isr 40
define_isr 41
define_isr 42
define_isr 43
define_isr 44
define_isr 45
define_isr 46
define_isr 47
define_isr 48
define_isr 49
define_isr 50
define_isr 51
define_isr 52
define_isr 53
define_isr 54
define_isr 55
define_isr 56
define_isr 57
define_isr 58
define_isr 59
define_isr 60
define_isr 61
define_isr 62
define_isr 63
define_isr 64
define_isr 65
define_isr 66
define_isr 67
define_isr 68
define_isr 69
define_isr 70
define_isr 71
define_isr 72
define_isr 73
define_isr 74
define_isr 75
define_isr 76
define_isr 77
define_isr 78
define_isr 79
define_isr 80
define_isr 81
define_isr 82
define_isr 83
define_isr 84
define_isr 85
define_isr 86
define_isr 87
define_isr 88
define_isr 89
define_isr 90
define_isr 91
define_isr 92
define_isr 93
define_isr 94
define_isr 95
define_isr 96
define_isr 97
define_isr 98
define_isr 99
define_isr 100
define_isr 101
define_isr 102
define_isr 103
define_isr 104
define_isr 105
define_isr 106
define_isr 107
define_isr 108
define_isr 109
define_isr 110
define_isr 111
define_isr 112
define_isr 113
define_isr 114
define_isr 115
define_isr 116
define_isr 117
define_isr 118
define_isr 119
define_isr 120
define_isr 121
define_isr 122
define_isr 123
define_isr 124
define_isr 125
define_isr 126
define_isr 127
