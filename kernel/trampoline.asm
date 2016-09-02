; 这段代码将被复制到0x7c000处，从实模式开始执行
; 注意寻址方式，这段代码复制到0x7c000处，但实模式下起始地址是0x7c00:0
; 也就是其实偏移地址是0，在引用相关符号的时候需要注意

; 进入保护方式的步骤
; 1. 关中断
; 2. 加载GDTR
; 3. 置CR0的PE位
; far jump

; 为何这段代码只能用16位方式编译才通过，但链接的时候，某些符号地址会被截断
; 例如lgdt指令的参数

global trampoline_entry
global pm_gdt
global pm_gdt_ptr
global pm_entry

extern initial_pml4t_low
extern tmp_gdt_ptr
extern ap_init

; 启动AP时BSP传递的参数
extern ap_id
extern ap_stack_top

KERNEL_VMA  equ 0xffff800000000000

[section .trampoline]
[BITS 16]

trampoline_entry:
    ; 关中断
    cli

    ; 段基址为0x7c000
    mov     ax, 0x7c00
    mov     ds, ax

    ; 显存段基址为0xa0000
    mov     ax, 0xa000
    mov     gs, ax

    ; 获取BSP传入的参数
    mov     bx, 0

    ; 终端上显示一个'X'
    mov     al, 'X'
    mov     ah, 0x4e
    mov     word [gs:bx], ax   ; 只有BX能作为index

    ; 加载GDT
    ; LGDT指令有lgdtw、lgdtd两种，当前为16位，采用实方式寻址
    lgdt    [pm_gdt_ptr]

    mov     al, '2'
    mov     ah, 0x4e
    mov     word [gs:bx+2], ax   ; 只有BX能作为index

    ; 启用保护方式，禁用分页
    mov     eax, cr0
    or      eax, 1              ; PE = 1
    and     eax, 0x7fffffff     ; PG = 0
    mov     cr0, eax

    mov     al, '3'
    mov     ah, 0x4e
    mov     word [gs:bx+4], ax   ; 只有BX能作为index

    jmp     dword 8:pm_entry

    jmp     $

[BITS 32]

ALIGN 8
pm_entry:
    ; 进入保护方式，首先设置段寄存器
    mov     ax, 16
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    mov     al, 'P'
    mov     ah, 0x4e
    mov     word [0xa0000], ax

    ; SMP架构下，所有核心都是完全一样的，BSP已经检测了相关支持，AP直接进行初始化就可以了

    ; 清ebp和eflags
    mov     ebp, 0
    push    0
    popf

    ; 加载页目录
    mov     edi, initial_pml4t_low
    mov     cr3, edi

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

    ; 加载长方式的GDT（目前为兼容方式，故加载的是32位GDTR）
    lgdt    [tmp_gdt_ptr]
    jmp     8:long_mode_entry

    jmp     $

[BITS 64]

long_mode_entry:
    ; 再次加载GDT，这次加载的是64位GDTR，且位于高地址
    mov     rax, tmp_gdt_ptr + KERNEL_VMA
    lgdt    [rax]
    mov     rax, higher_half + KERNEL_VMA
    jmp     rax

    jmp     $

higher_half:
    ; 初始化段寄存器
    mov     ax, 16
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    ; 加载内核栈
    mov     rsp, qword ap_stack_top

    ; 初始化FS和GS(can be used as thread local storage)
    xor     rax, rax
    mov     ecx, 0xc0000100
    wrmsr                   ; FS.base = 0
    mov     ecx, 0xc0000101
    wrmsr                   ; GS.base = 0

    jmp     $

ALIGN 8
pm_gdt:
.null   equ $ - pm_gdt
    dd      0
    dd      0
.code0  equ $ - pm_gdt
    dw      0xffff              ; limit [0:15]
    dw      0                   ; base [0:15]
    db      0                   ; base [16:23]
    db      10011010b           ; Present, DPL=0, non-conforming
    db      11001111b           ; 32-bit, and limit[16:19]
    db      0                   ; base [24:31]
.data0  equ $ - pm_gdt
    dw      0xffff              ; limit [0:15]
    dw      0                   ; base [0:15]
    db      0                   ; base [16:23]
    db      10010010b           ; Present, DPL=0, writable
    db      11001111b           ; limit [16:19] and attr
    db      0                   ; base [24:31]

pm_gdt_ptr  equ $ - trampoline_entry
    dw      $ - pm_gdt - 1  ; limit
    dd      pm_gdt          ; base