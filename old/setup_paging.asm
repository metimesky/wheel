[section .text]
[BITS 32]
    ; clear 16KB of memory for 4 page tables, which covers 2MB address space.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 4096
    rep     stosd
    mov     edi, cr3

    ; setup Page-Map Level-4 (PML4) Table, only 1 entry
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; setup Page Directory Pointer (PDP) Table, only 1 entry
    mov     dword [edi], 0x2003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 0x1000

    ; setup Page Directory (PD) Table, only 1 entry
    mov     dword [edi], 0x3003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+12K (Page Table)
    add     edi, 0x1000

    ; setup Page Table, 512 entries
    mov     ebx, 0x00000003         ; present, read/write
    mov     ecx, 512
.set_entry:
    mov     dword [edi], ebx
    add     ebx, 0x1000
    add     edi, 8
    loop    .set_entry

[section .bss]
; reserve 16KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x4000



;=======================================

    ; PDP 512
    mov     ebx, 0x00000083         ; present, read/write, 1G granularity
    mov     ecx, 512                ; 512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 0x1000
    add     edi, 8
    loop    .set_pdp_entry