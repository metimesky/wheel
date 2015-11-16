## Initial Paging

GRUB 将控制权交给内核之后，内核首先进行 64 位模式的切换。由于 64 位模式需要启用分页，因此内核需要具有一定初始分页。

初始分页直接在 `boot.asm` 中，使用 2MB 大小的页，因为这种大小的页所有 64 位 CPU 都支持，而且最节省页表空间。

AMD64 requires PAE paging, which means the large page is 2MB rather than 4MB and page entry size is doubled.

According to the manual, page size in AMD64 can be 4K, 2M and 1G. However, if I use 1G pages, the testing Thinkpad will crash and auto-reboot.

AMD64 requires PAE which can handle 2MB pages, indicating all 64-bit CPU can handle 2MB pages. But for 1GB pages, only on some CPU are valid. The support for 1GB page can be identified by looking at CPUID 0x80000001, EDX bit 26

``` asm
[section .text]
[BITS 32]
    ; clear 16KB of memory for 4 page tables, which covers 2MB address space.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 4096
    rep     stosd
    mov     edi, cr3

    ; creating 1 Page-Map Level-4 Entry (PML4E)
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; creating 1 Page Directory Pointer Entry (PDPE)
    mov     dword [edi], 0x2003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 0x1000

    ; creating 1 Page Directory Entry (PDE)
    mov     dword [edi], 0x3003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+12K (Page Table)
    add     edi, 0x1000

    ; creating 512 Page Table Entry (PTE)
    mov     ebx, 0x00000003         ; present, read/write
    mov     ecx, 512
.set_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 12
    add     edi, 8
    loop    .set_entry

[section .bss]
; reserve 16KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x4000
```

512 entries of 2MB pages, identically mapping 1G memory:

``` asm
[section .text]
[BITS 32]
    ; clear 12KB of memory for 3 page tables, which covers 1GB address space.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 3072
    rep     stosd
    mov     edi, cr3

    ; creating 1 Page-Map Level-4 Entry (PML4)
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; creating 1 Page Directory Pointer Entry (PDPE)
    mov     dword [edi], 0x2003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+8K (PD Table)
    add     edi, 0x1000

    ; creating 512 Page Directory Entry (PDE)
    mov     ebx, 0x00000083         ; present, read/write, 1G granularity
    mov     ecx, 512                ; 512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 21
    add     edi, 8
    loop    .set_pdp_entry

[section .bss]
; reserve 12KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x3000
```

512 entries of 1GB pages, identically mapping 512G memory:

```
[section .text]
[BITS 32]
    ; clear 8KB of memory for 2 page tables, which covers 512GB address space.
    mov     edi, pml4t
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 2048
    rep     stosd
    mov     edi, cr3

    ; creating 1 Page-Map Level-4 Entry (PML4E)
    mov     dword [edi], 0x1003     ; present, read/write
    add     dword [edi], pml4t      ; pointing to pml4t+4K (PDP Table)
    add     edi, 0x1000

    ; creating 512 Page Directory Pointer Entry (PDP)
    mov     ebx, 0x00000083         ; present, read/write, 1G granularity
    mov     ecx, 512                ; 512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 30
    add     edi, 8
    loop    .set_pdp_entry

[section .bss]
; reserve 8KB for page tables.
ALIGN 0x1000
pml4t:          resb 0x2000
```
