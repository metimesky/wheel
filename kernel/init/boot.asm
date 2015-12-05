; This is the entry point of kernel, the control is handed over from GRUB.
; In this file, we first setup 64-bit mode, including GDT and PAE paging,
; then jump into C code.
; Besides all the setting up stuffs, this file also include the definition
; of initial kernel stack, GDT and initial page tables.

global gdt
global kernel_stack_top
global pml4t

extern  kernel_start
extern  kernel_data_end
extern  kernel_bss_end

extern  init

MB1_MAGIC   equ 0x1badb002              ; magic number
MB1_FLAGS   equ 1<<0|1<<1|1<<16         ; aligned, mem info, address info valid
MB1_CHECK   equ -(MB1_MAGIC+MB1_FLAGS)

[section .boot]
[BITS 32]
    jmp     multiboot_entry
ALIGN 4
multiboot_header:
    dd      MB1_MAGIC
    dd      MB1_FLAGS
    dd      MB1_CHECK
    dd      multiboot_header    ; header_addr
    dd      kernel_start        ; load_addr
    dd      kernel_data_end     ; load_end_addr
    dd      kernel_bss_end      ; bss_end_addr
    dd      multiboot_entry     ; entry_addr

multiboot_entry:
    ; disable interrupt
    cli

    ; store boot information
    mov     [mb_eax], eax
    mov     [mb_ebx], ebx

    ; setup kernel stack
    mov     esp, kernel_stack_top

    ; clear ebp and eflags
    mov     ebp, 0
    push    0
    popf

    ; check whether CPUID is supported by setting bit 21 of eflags
    pushfd
    pop     eax
    mov     ecx, eax
    xor     eax, 1 << 21
    push    eax
    popfd
    pushfd
    pop     eax
    push    ecx
    popfd
    xor     eax, ecx
    jz      no_long_mode    ; CPUID not supported, thus no long mode
    
    ; check CPUID extended functions
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    jb      no_long_mode    ; CPUID extended functions not available, thus no long mode
    
    ; check long mode existence
    mov     eax, 0x80000001
    cpuid
    test    edx, 1 << 29
    jz      no_long_mode

enter_long_mode:
    ; disable paging (to be safe)
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax

    ; clear 24KB of memory for 6 page tables, which covers 4GB address space.
    ; we need to cover 4GB because mem mapped devices like APIC are in 3~4G.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 6144
    rep     stosd
    mov     edi, cr3

    ; creating 1 Page-Map Level-4 Entry (PML4)
    mov     dword [edi], 0x1007     ; present, read/write, user
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; creating 4 Page Directory Pointer Entry (PDPE)
    mov     dword [edi], 0x2007     ; present, read/write, user
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 8
    mov     dword [edi], 0x3007     ; present, read/write, user
    add     dword [edi], pml4t      ; pointing to pml4t+12K
    add     edi, 8
    mov     dword [edi], 0x4007     ; present, read/write, user
    add     dword [edi], pml4t      ; pointing to pml4t+12K
    add     edi, 8
    mov     dword [edi], 0x5007     ; present, read/write, user
    add     dword [edi], pml4t      ; pointing to pml4t+12K
    add     edi, 0x1000 - 24

    ; creating 4*512 Page Directory Entry (PDE)
    mov     ebx, 0x00000087         ; present, read/write, 2MB granularity
    mov     ecx, 4*512              ; 4*512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 21
    add     edi, 8
    loop    .set_pdp_entry

    ; enable PAE-paging
    mov     eax, cr4
    or      eax, 1 << 5
    mov     cr4, eax

    ; set LM-bit
    mov     ecx, 0xc0000080
    rdmsr
    or      eax, 1 << 8
    wrmsr

    ; enable paging
    mov     eax, cr0
    or      eax, 1 << 31
    mov     cr0, eax

    ; enter 64-bit submode
    lgdt    [gdt_ptr]

    ; jump to real 64-bit code
    jmp     gdt.code0:long_mode_entry

    ; should never return here
    hlt
    jmp     $

no_long_mode:
    ; write error message
    mov     edi, 0xb8000
    mov     esi, .err_msg
    mov     ecx, .err_msg_len
    mov     ah, 0x4e        ; yellow on red
.print:
    lodsb
    stosw
    loop    .print

    ; halt the machine
    hlt
    jmp     $

.err_msg        db      "CPU is not compatible with 64-bit mode."
.err_msg_len    equ     $ - .err_msg

[BITS 64]
long_mode_entry:
    mov     ax, gdt.data0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    mov     edi, [mb_eax]
    mov     esi, [mb_ebx]
    call    init    ; call the main function of kernel

    ; halt on return
    hlt
    jmp     $

[section .data]
[BITS 32]
; temporary GDT (64-bit).
gdt:
.null   equ $ - gdt
    dq      0
.code0  equ $ - gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10011000b       ; Present, DPL=0, non-conforming
    db      10100000b       ; 64-bit, and limit[16:19]
    db      0               ; base [24:31]
.data0  equ $ - gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      10010010b       ; Present, DPL=0, writable
    db      11000000b       ; limit [16:19] and attr
    db      0               ; base [24:31]
.code3  equ $ - gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      11111000b       ; Present, DPL=0, non-conforming
    db      10100000b       ; 64-bit, and limit[16:19]
    db      0               ; base [24:31]
.data3  equ $ - gdt
    dw      0               ; limit [0:15]
    dw      0               ; base [0:15]
    db      0               ; base [16:23]
    db      11110010b       ; Present, DPL=0, writable
    db      11000000b       ; limit [16:19] and attr
    db      0               ; base [24:31]
.tss    equ $ - gdt         ; system-segment descriptor is 16 bytes long
    dq      0
    dq      0

gdt_ptr:
    dw      $ - gdt - 1     ; limit
    dq      gdt             ; base

; temporary store the multiboot info
mb_eax: dd  0
mb_ebx: dd  0

[section .bss]
; reserve 4KB for kernel stack.
kernel_stack:   resb 0x1000
kernel_stack_top:

; reserve 24KB for page tables. (1 PML4T, 1 PDPT, 4 PDT)
ALIGN 0x1000
pml4t:          resb 0x6000