Wheel
=====

Wheel is an operating system written from scratch. Currently Wheel is only compatible with AMD64 CPU.

### Code Structure

OS is complex with its component link to each other, so it's hard to organize the source code cleanly.

maybe we can make a directory called `basic`, containing low level stuff like interrupt entry. or we can simple call that directory `kernel`

Let there be a folder `arch`. And each of its sub-folder if for a target platform. One of them is `univeral`, which contains code that is arch-independent.

In `arch/amd64/lib.{asm,c}`, we have write:

```
global _arch_amd64_memset
_arch_amd64_memset:
    pass
    ret
```

In the header file, we can write:

``` h
#ifndef memset
    #define memset _arch_amd64_memset
#endif // memset
```

### Dev

Wheel is written in Assembly and C, developed using YASM and Clang under Linux environment. GNU-LD is used as linker, MTools is used to read/write floppy dist image. Bochs and QEMU are employed for testing.

You can use the following command to install all the programs needed:

~~~
sudo apt-get install build-essential yasm clang mtools qemu-kvm
~~~

### Starting procedure

GRUB handles control to `init/boot.asm`, it then execute `init/stage0.asm`, which checks the long mode, setup stack, paging and temporary GDT. then jumps into `init/stage1.c`, which setup the real GDT and IDT(WIP).

### Paging

AMD64 requires PAE paging, which means the large page is 2MB rather than 4MB and page entry size is doubled.

According to the manual, page size in AMD64 can be 4K, 2M and 1G. However, if I use 1G pages, the testing Thinkpad will crash and auto-reboot.

Different initial paging strategy is provided.

512 entries of 4KB pages, identically mapping 2M memory:

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

``` asm
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