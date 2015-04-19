extern  exception_dispatcher
extern  interrupt_dispatcher

extern  kernel_stack_top
extern  process_to_go
extern  tss

[SECTION    .text]
[BITS       32]

%macro exception_with_errcode 1
global  exp%1
exp%1:
    push    %1
    call    exception_dispatcher
    add     esp, 8
    iretd
%endmacro

%macro exception_no_errcode 1
global  exp%1
exp%1:
    push    0xffffffff
    push    %1
    call    exception_dispatcher
    add     esp, 8
    iretd
%endmacro

exception_no_errcode   0
exception_no_errcode   1
exception_no_errcode   2
exception_no_errcode   3
exception_no_errcode   4
exception_no_errcode   5
exception_no_errcode   6
exception_no_errcode   7
exception_with_errcode 8
exception_no_errcode   9
exception_with_errcode 10
exception_with_errcode 11
exception_with_errcode 12
exception_with_errcode 13
exception_with_errcode 14
exception_no_errcode   15
exception_no_errcode   16
exception_with_errcode 17
exception_no_errcode   18
exception_no_errcode   19
exception_no_errcode   20

%macro save_context 0
    pushad
    push    ds
    push    es
    push    fs
    push    gs
%endmacro

%macro restore_context 0
    pop     gs
    pop     fs
    pop     es
    pop     ds
    popad
%endmacro

%macro interrupt 1
global  irq%1
irq%1:
    ; when interrupt occurs, esp is pointing to pcb
    save_context

    ; mask current interrupt, and send EOI
    %if %1 < 8      ; master chip
        in      al, 0x21
        or      al, (1 << %1)
        out     0x21, al

        mov     al, 0x20
        out     0x20, al
    %else           ; slave chip
        in      al, 0xa1
        or      al, (1 << (%1-8))
        out     0xa1, al

        mov     al, 0x20
        out     0x20, al
        out     0xa0, al
    %endif

    ; switch to kernel stack
    mov     esp, kernel_stack_top

    ;sti
    ;push    %1
    call    interrupt_dispatcher
    ;add     esp, 4
    ;cli

    ; switch back to pcb
    mov     esp, [process_to_go]
    mov     eax, esp
    add     eax, 68
    mov     [tss+4], eax        ; set tss->esp0 to the pcb of current process

    ; unmask current interrupt
    %if %1 < 8      ; master chip
        in      al, 0x21
        and     al, ~(1 << %1)
        out     0x21, al
    %else           ; slave chip
        in      al, 0xa1
        and     al, ~(1 << (%1-8))
        out     0xa1, al
    %endif
    restore_context
    iretd
%endmacro

interrupt   0
interrupt   1
interrupt   2
interrupt   3
interrupt   4
interrupt   5
interrupt   6
interrupt   7
interrupt   8
interrupt   9
interrupt   10
interrupt   11
interrupt   12
interrupt   13
interrupt   14
interrupt   15
