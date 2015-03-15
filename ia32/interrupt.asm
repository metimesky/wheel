;; there are 3 classes of interrupts on IA-32:
;; 1. Exception -- generated internally by the CPU.
;; 2. Interrupt Request -- generated externally by the chipset

global  exp0
global  exp1
global  exp2
global  exp3
global  exp4
global  exp5
global  exp6
global  exp7
global  exp8
global  exp9
global  exp10
global  exp11
global  exp12
global  exp13
global  exp14
global  exp15
global  exp16
global  exp17
global  exp18
global  exp19
global  exp20
extern  exception_handler

;global  irq0
;global  irq1
;global  irq2
;global  irq3
;global  irq4
;global  irq5
;global  irq6
;global  irq7
;global  irq8
;global  irq9
;global  irq10
;global  irq11
;global  irq12
;global  irq13
;global  irq14
;global  irq15

[SECTION    .text]
[BITS       32]

;; exception
exp0:   ; #DE divide error
    push    0xffffffff  ; no error code
    push    0
    jmp     exception

exp1:   ; #DB single step exception
    push    0xffffffff  ; no error code
    push    1
    jmp     exception

exp2:   ; NMI
    push    0xffffffff  ; no error code
    push    2
    jmp     exception

exp3:   ; #BP breakpoint exception
    push    0xffffffff  ; no error code
    push    3
    jmp     exception

exp4:   ; #OF overflow
    push    0xffffffff  ; no error code
    push    4
    jmp     exception

exp5:   ; #BR bound range exceeded
    push    0xffffffff  ; no error code
    push    5
    jmp     exception

exp6:   ; #UD undefined opcode
    push    0xffffffff  ; no error code
    push    6
    jmp     exception

exp7:   ; #NM no math coprocessor
    push    0xffffffff  ; no error code
    push    7
    jmp     exception

exp8:   ; #DF double fault
    push    8
    jmp     exception

exp9:   ; coprocessor segment overrun (reserved)
    push    0xffffffff  ; no error code
    push    9
    jmp     exception

exp10:  ; #TS invalid TSS
    push    10
    jmp     exception

exp11:  ; #NP segment not present
    push    11
    jmp     exception

exp12:  ; #SS stack segment fault
    push    12
    jmp     exception

exp13:  ; #GP general protection
    push    13
    jmp     exception

exp14:  ; #PF page fault
    push    14
    jmp     exception

exp15:  ; reserved
    push    0xffffffff
    push    15
    jmp     exception

exp16:  ; #MF math fault
    push    0xffffffff
    push    16
    jmp     exception

exp17:  ; #AC alignment check
    push    17
    jmp     exception

exp18:  ; #MC machine check
    push    0xffffffff
    push    18
    jmp     exception

exp19:  ; #XM SIMD floating point exception
    push    0xffffffff
    push    19
    jmp     exception

exp20:  ; #VE virtualization exception
    push    0xffffffff
    push    20
    jmp     exception

exception:
    call    exception_handler
    add     esp, 8
    jmp     $

;; interrupt
%macro irq_master 2
    call    save
    in      al, INT_M_CTLMASK
    or      al, (1 << %1)
    out     INT_M_CTLMASK, al

    mov     al, EOI
    out     INT_M_CTL, al

    sti
    call    %2
    pop ecx
    cli

    in  al, INT_M_CTLMASK
    and al, ~(1 << %1)
    out INT_M_CTLMASK, al

    ret
%endmacro


