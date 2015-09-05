; i don't know whether it is right to put this
; file in the `init` folder.
; This file contains all the entry point of each interrupt vecters.

extern interrupt_handler_table

; interrupt with no error code, pushes a 0 error code
%macro define_isr 1
global isr%1
isr%1:
    push    0xffffffffffffffff
    push    %1
    common_int_handler %1
%endmacro

; interrupt with an error code
%macro define_isr_e 1
global isr%1
isr%1:
    push    %1
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
; NOTICE this is a macro which looks like a function
%macro common_int_handler 1
    ; check if we are comming from user-mode
    ; testl   $3, 24(%rsp)
    ; jnz     common_user_isr_handler

    ; push the rest of the interrupt frame to the stack
    save_regs

    cld

    ; save frame pointer to rbp
    mov    rbp, rsp

    ; Set the RF (resume flag) in RFLAGS. This prevents an instruction
    ; breakpoint on the instruction we're returning to to trigger a debug
    ; exception.
    ; orq     $X86_EFLAGS_RESUME, IFRAME_flags(%rbp)

    ; sub     rsp, 512
    ; and     rsp, ~15    ; clear lowest 4 bits
    ;fxsaveq (%rsp)

    ; Call the interrupt handler.
    mov     rdi, rbp
    call    [interrupt_handler_table + 8 * %1]

    ;fxrstorq    (%rsp)

    ; restore saved frame pointer from rbp
    mov     rsp, rbp

    ; restore the saved registers.
    restore_regs

    ; skip error code and vector number
    add     rsp, 16

    iretq
%endmacro

[section .text]
[BITS 64]

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

; hardware interrupts
define_isr 32
define_isr 33
define_isr 34
define_isr 35