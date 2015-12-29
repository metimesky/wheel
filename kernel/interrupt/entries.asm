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


PIC_BASE equ 32
LVT_BASE equ 48

extern interrupt_handler_table

[BITS 64]

; internal exception with no error code, pushes a dummy error code
%macro define_isr 1
global isr%1
isr%1:
    push    0xffffffffffffffff
    common_int_handler %1
%endmacro

; internal exception with an error code
%macro define_isr_e 1
global isr%1
isr%1:
    common_int_handler %1
%endmacro

; external interrupt from 8259 PIC
%macro define_pic_irq 1
global isr%1
isr%1:
    push    0xffffffffffffffff

    ; sending PIC EOI
    mov     al, 0x20
    %if (%1 - PIC_BASE) < 8
        out     0x20, al
    %else
        out     0x20, al
        out     0xa0, al
    %endif

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
; Exceptions
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

; old PIC IRQs (master)
define_isr 32
define_isr 33
define_isr 34
define_isr 35
define_isr 36
define_isr 37
define_isr 38
define_isr 39

; old PIC IRQs (slave)
define_isr 40
define_isr 41
define_isr 42
define_isr 43
define_isr 44
define_isr 45
define_isr 46
define_isr 47

; IO APIC IRQs
;define_isr 48

; local APIC LVT (currently only timer is used)
define_isr 48   ; CMCI
define_isr 49   ; timer
define_isr 50   ; thermal monitor
define_isr 51   ; performance counter
define_isr 52   ; LINT0
define_isr 53   ; LINT1
define_isr 54   ; error