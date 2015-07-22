;; this file is the entry point of an interrupt.
; TERMS:
;   ISR: Interrupt service routine, handler for exceptions
;   IRQ: Interrupt Request, handler for hardware interrupts
;-------------------------------------------------------------------------------
; This file does not distinguish hardware interrupts and software exceptions
; the difference is handled in C functions.

; array of C function pointers
extern interrupt_handler_table

[BITS 64]

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

; macro to define exception routine entry
; arg[0] -- vector number
; arg[1] -- has errcode
%macro exception_entry 2
global exception_entry%1
exception_entry%1:
%if %2  ; if this interrupt has error code
    pop     rdi
%else   ; if this interrupt has no error code
    ;mov     rdi, 0xffffffffffffffff
    push    0xffffffffffffffff
%endif

    save_regs    ; save the context from register
    
    mov     rdi, rsp
    add     rdi, 88
    call    [interrupt_handler_table + 8 * %1]

    restore_regs ; restore context to register

    iretq
    ;db      0x48, 0xcf
%endmacro

; irq0 map to vector 32
IRQ_BASE    equ 32

; macro to define hardware interrupt routine entry
; arg[0] -- interrupt number (IRQ)
%macro hw_int_entry 1
global hw_int_entry%1
hw_int_entry%1:
    ; push a fake error code to keep stack aligned
    push    0xffffffffffffffff

    ;save    ; save the context from register

    ; send EOI
    mov     al, 0x20
%if %1 < 8
    out     0x20, al
%else
    out     0x20, al
    out     0xa0, al
#endif

    mov     rdi, rsp
    add     rdi, 88
    call    [interrupt_handler_table + 8 * (%1 + IRQ_BASE)]

    ;restore ; restore context to register

    iretq
    ;db      0x48, 0xcf
%endmacro

[section .text]
[BITS 64]
; exception_entry  0, 0       ; #DE divided-by-zero-error exception, no errcode
; exception_entry  1, 0       ; #DB debug exception, no errcode
; exception_entry  2, 0       ; NMI non-maskable-interrupt exception, no errcode
; exception_entry  3, 0       ; #BP breakpoint exception, no errcode
; exception_entry  4, 0       ; #OF overflow exception, no errcode
; exception_entry  5, 0       ; #BR bound-range exception, no errcode
; exception_entry  6, 0       ; #UD invalid-opcode exception, no errcode
; exception_entry  7, 0       ; #NM device-not-available exception, no errcode
; exception_entry  8, 1       ; #DF double-fault exception, errcode = 0
; exception_entry  9, 0       ; coprocessor-segment-overrun exception, reserved
; exception_entry 10, 1       ; #TS invalid-TSS exception
; exception_entry 11, 1       ; #NP segment-not-present exception
; exception_entry 12, 1       ; #SS stack exception
; exception_entry 13, 1       ; #GP general-protection exception
; exception_entry 14, 1       ; #PF page-fault exception
; exception_entry 15, 0       ; reserved
; exception_entry 16, 0       ; #MF x87 floating-point exception-pending, no errcode
; exception_entry 17, 1       ; #AC alignment-check exception, errcode = 0
; exception_entry 18, 0       ; #MC machine-check exception, no errcode
; exception_entry 19, 0       ; #XF SIMD floating-point exception, no errcode
; exception_entry 20, 0
; 
; exception_entry 30, 1       ; #SX security exception, errcode = 1

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

; ; external interrupts
; hw_int_entry 0              ; clock
; hw_int_entry 1              ; keyboard
; hw_int_entry 2              ; clock
; hw_int_entry 3              ; clock
; hw_int_entry 4              ; clock
; 
; 