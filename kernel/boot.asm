global multiboot_entry

extern __early_pgtbl_low
extern kernel_stack_top

extern init

; multiboot spec version 1
MB1_MAGIC   equ 0x1badb002          ; multiboot 1 magic number
MB1_FLAGS   equ 1<<0|1<<1           ; 4K-aligned, mem info

; 内核虚拟地址基地址
KERNEL_VMA  equ 0xffff800000000000

[section .boot]
[BITS 32]

    ; 跳转到真正的入口点
    jmp multiboot_entry

ALIGN 4
multiboot1_header:
    dd      MB1_MAGIC
    dd      MB1_FLAGS
    dd      -(MB1_MAGIC+MB1_FLAGS)

ALIGN 4
multiboot_entry:
    cli     ; 关中断

    ; 保存GRUB的引导信息
    mov     [mb_eax], eax
    mov     [mb_ebx], ebx

    ; 清ebp和eflags
    mov     ebp, 0
    push    0
    popf

    ; 检查是否支持CPUID扩展功能
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    jb      no_long_mode        ; 不支持

    ; 检查是否支持长方式
    mov     eax, 0x80000001
    cpuid
    test    edx, 1 << 29
    jz      no_long_mode        ; 不支持

enter_long_mode:
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax            ; 关分页

    ; clear 24KB of memory for 6 page tables, which covers 4GB address space.
    ; we need to cover 4GB because mem mapped devices like APIC are in 3~4G.
    mov     edi, __early_pgtbl_low
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 6144
    rep     stosd
    mov     edi, cr3

    ; creating 1 Page-Map Level-4 Entry (PML4E)
    ; 两段地址前4GB均映射到物理地址0~4G
    mov     eax, 0x0007                     ; present, read/write, user
    add     eax, __early_pgtbl_low + 0x1000 ; point to pml4t + 4K
    mov     [edi], eax                      ; write to pml4t[0]
    mov     [edi+0x800], eax                ; write to pml4t[256]
    add     edi, 0x1000

    ; creating 4 Page Directory Pointer Entries (PDPE)
    mov     dword [edi], 0x2007             ; present, read/write, user
    add     dword [edi], __early_pgtbl_low  ; pointing to pml4t+8K (PD Table)
    add     edi, 8
    mov     dword [edi], 0x3007             ; present, read/write, user
    add     dword [edi], __early_pgtbl_low  ; pointing to pml4t+12K
    add     edi, 8
    mov     dword [edi], 0x4007             ; present, read/write, user
    add     dword [edi], __early_pgtbl_low  ; pointing to pml4t+16K
    add     edi, 8
    mov     dword [edi], 0x5007             ; present, read/write, user
    add     dword [edi], __early_pgtbl_low  ; pointing to pml4t+20K
    add     edi, 0x1000 - 24

    ; creating 4*512 Page Directory Entries (PDE)
    mov     ebx, 0x00000087         ; present, read/write, 2MB granularity
    mov     ecx, 4*512              ; 4*512 entries in total
.set_pdp_entry:
    mov     dword [edi], ebx
    add     ebx, 1 << 21
    add     edi, 8
    loop    .set_pdp_entry

    ; 启用PAE分页
    mov     eax, cr4
    or      eax, 1 << 5
    mov     cr4, eax

    ; 置LM位
    mov     ecx, 0xc0000080
    rdmsr
    or      eax, 1 << 8
    wrmsr

    ; 开分页，CPU进入长方式（兼容方式）
    mov     eax, cr0
    or      eax, 1 << 31
    mov     cr0, eax

    ; After enabling PAE paging, the CPU is switched into long mode.
    ; more precisely, in compatibility sub-mode.

    ; since we are in compatibility sub-mode, this loads 32 bit GDT
    lgdt    [tmp_gdt_ptr]
    jmp     tmp_gdt.code0:long_mode_entry

    ; should never arrive here
    jmp     $

no_long_mode:
    ; 不支持长方式，输出错误信息并停机
    mov     edi, 0xb8000
    mov     esi, .err_msg
    mov     ecx, .err_msg_len
    mov     ah, 0x4e            ; 红底黄字
.print:
    lodsb
    stosw
    loop    .print

    ; 停机
    hlt
    jmp     $

.err_msg        db      "CPU is not compatible with 64-bit mode."
.err_msg_len    equ     $ - .err_msg

[BITS 64]

long_mode_entry:
    ; 现已进入长方式
    ; load GDT again, this time load 64 bit GDT, and jump to higher half
    mov     rax, tmp_gdt_ptr + KERNEL_VMA
    lgdt    [rax]
    mov     rax, higher_half + KERNEL_VMA
    jmp     rax

    jmp     $

higher_half:
    ; init segment registers (although they are ignored)
    mov     ax, tmp_gdt.data0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    ; 加载内核栈
    mov     rsp, qword kernel_stack_top

    ; init fs and gs (can be used as thread local storage)
    xor     rax, rax
    mov     ecx, 0xc0000100
    wrmsr                   ; FS.base = 0
    mov     ecx, 0xc0000101
    wrmsr                   ; GS.base = 0

    ; retrieve bootloader information
    mov     edi, dword [mb_eax]     ; auto zero upper 32-bit half
    mov     esi, dword [mb_ebx]     ; auto zero upper 32-bit half

    ; disable lower half mapping
    ; 现在还不能将0~4G的映射禁用，因为AP启动时需要
    ;mov     qword [__early_pgtbl_low], 0
    ;invlpg  [0]

    ; begin executing C code in higher half
    xor     rbp, rbp
    mov     rax, qword init
    call    rax                     ; call the main function of kernel

    ; should never return here - halt on return
    hlt
    jmp     $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; temporary GDT (64-bit).
ALIGN 8
tmp_gdt:
.null   equ $ - tmp_gdt
    dq      0
.code0  equ $ - tmp_gdt
    dw      0                   ; limit [0:15]
    dw      0                   ; base [0:15]
    db      0                   ; base [16:23]
    db      10011000b           ; Present, DPL=0, non-conforming
    db      10100000b           ; 64-bit, and limit[16:19]
    db      0                   ; base [24:31]
.data0  equ $ - tmp_gdt
    dw      0                   ; limit [0:15]
    dw      0                   ; base [0:15]
    db      0                   ; base [16:23]
    db      10010010b           ; Present, DPL=0, writable
    db      11000000b           ; limit [16:19] and attr
    db      0                   ; base [24:31]

tmp_gdt_ptr:
    dw      $ - tmp_gdt - 1         ; limit
    dq      tmp_gdt + KERNEL_VMA    ; base

; temporary store the multiboot info
mb_eax: dd  0
mb_ebx: dd  0
