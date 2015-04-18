global  in_byte
global  out_byte
global  in_word
global  out_word
global  in_dword
global  out_dword
global  io_wait

global  load_gdtr
global  load_idtr
global  load_tr

global  goto_ring3

[SECTION    .text]
; uint8_t in_byte(uint16_t port);
in_byte:
    mov     edx, [esp+4]
    xor     eax, eax
    in      al, dx
    ret

; void out_byte(uint16_t port, uint8_t data);
out_byte:
    mov     edx, [esp+4]
    mov     eax, [esp+8]
    out     dx, al
    ret

; uint16_t in_word(uint16_t port);
in_word:
    mov     edx, [esp+4]
    xor     eax, eax
    in      ax, dx
    ret

; void out_word(uint16_t port, uint16_t data);
out_word:
    mov     edx, [esp+4]
    mov     eax, [esp+8]
    out     dx, ax
    ret

; uint32_t in_dword(uint16_t port);
in_dword:
    mov     edx, [esp+4]
    xor     eax, eax
    in      eax, dx
    ret

; void out_dword(uint16_t port, uint32_t data);
out_dword:
    mov     edx, [esp+4]
    mov     eax, [esp+8]
    out     dx, eax
    ret

; void io_wait();
io_wait:
    xor     eax, eax
    out     0x80, al
    ret

; void load_gdtr(gdt_ptr_t*);
; this routine expects 0x08 to be kernel code
; segment and 0x10 to be kernel data segment
load_gdtr:
    mov     eax, [esp+4]
    lgdt    [eax]
    jmp     0x08:.flush_gdt
.flush_gdt:
    mov     ax, 0x10
    mov     ds, eax
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    ret

; void load_idtr(idt_ptr_t*);
load_idtr:
    mov     eax, [esp+4]
    lidt    [eax]
    ret

; void load_tr(uint16_t sel);
load_tr:
    mov     eax, [esp+4]
    ltr     ax
    ret

extern  tss
extern  user_mode
extern  kernel_stack_top
goto_ring3:
    mov     eax, esp
    mov     dword[tss+8], 0x10
    mov     dword[tss+4], kernel_stack_top    ; save esp0
    mov     eax, 0x23
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    push    0x23        ; target ss
    mov     eax, esp
    push    eax         ; target esp
    push    0x1202      ; target eflags, IF=1
    push    0x1b        ; target cs, user-code, RPL=3
    push    user_mode   ; target eip
    iretd