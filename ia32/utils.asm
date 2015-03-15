global  load_gdtr
global  load_idtr
global  load_tr

global  in_byte
global  out_byte

[SECTION    .text]
[BITS       32]

; void load_gdtr(gdt_ptr_t*);
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

; uint8_t in_byte(uint16_t port);
in_byte:
    mov     edx, [esp+4]
    xor     eax, eax
    in      al, dx
    jmp     .wait
.wait:
    ret

; void out_byte(uint16_t port data);
out_byte:
    mov     edx, [esp+4]
    mov     eax, [esp+8]
    out     dx, al
    jmp     .wait
.wait:
    ret

