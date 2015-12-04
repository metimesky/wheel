# 内存管理

Wheel 的内存管理分为两个层次，底层是物理内存管理，上层是虚拟内存管理。

物理内存管理实现物理页的分配和释放，分页的操作也由物理内存管理部分实现，包括地址映射的建立和清除。虚拟地址管理使用物理内存管理器提供的功能，实现对象级别的内存管理，也就是提供 `malloc` 和 `free` 的功能。

## 初始分页

内存管理的硬件支持是分页机制。在 64 位模式下，分页是强制的，而且是 PAE 分页，PAE 分页意味着支持 2MB 大小的页。在 Wheel 操作系统启动过程中，切换 64 位模式是最先执行的动作。因此在内存管理模块未启动的时候就需要建立分页。

初始分页就是在切换 64 位模式的时候制定的分页映射。虽然当内存管理模块初始化之后可以改变页表，但是 Wheel 创建的初始分页是不变的。

首先需要明确内核的虚拟地址空间结构。64 位 CPU 使用的虚拟地址称作 Canonical Address Form，将完整的 52 位空间分成高低两个部分，更高位是符号扩展。

内核首先建立 0~4GB 的对等映射，也就是内核虚拟地址空间中，0~4GB 映射到 0~4GB 的物理内存。因为许多内存映射设备都处在物理地址 3~4GB 处，而且 52 位的线性地址空间足够使用，并且对等映射非常方便。这部分的映射在切换 64 位方式的时候设置，作为内核的初始分页。

下面是三种建立映射的汇编代码，分别是用了 4KB 页、2MB 页，和 1GB 页。其中 1GB 页在某些 CPU 上不支持，2MB 页在所有 64 位 CPU 上都支持，因此 Wheel 默认使用 2MB 的页实现 0~4GB 的映射。

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
