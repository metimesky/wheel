; 本文件定义了中断处理函数的入口点，真正的实现在int_handler_table数组中
; 目前不支持中断重入

; 定义在idt.c中
extern int_handler_table

; 定义在scheduler中
;extern target_rsp
extern __percpu_rsp_scratch
extern __percpu_offset

[section .text]
[BITS 64]

; 不含错误码的ISR
%macro define_isr 1
global isr%1
isr%1:
    push    -1              ; sign-extended to 64bit
    common_int_handler %1
%endmacro

; 含错误码的ISR
%macro define_isr_e 1
global isr%1
isr%1:
    common_int_handler %1
%endmacro

; 在ss/rsp/rflags/cs/rip和错误码之后，保存其他寄存器
%macro save_regs 0
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rdi
    push    rsi
    push    rbp
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
%endmacro

; 恢复寄存器
%macro restore_regs 0
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    pop     rsi
    pop     rdi
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax
%endmacro

; 通用中断处理逻辑
%macro common_int_handler 1
    ; 保存上下文（通用寄存器）
    save_regs

    ; 读取percpu_ptr(rsp_scratch)到r15
    ; r15在调用C函数时会被保持
    mov     ecx, 0xc0000101
    rdmsr                           ; 读取GS.BASE，保存在eax中
    mov     rdx, qword __percpu_offset
    mul     qword [rdx]             ; rax = __percpu_offset * cpu_id
    mov     r15, qword __percpu_rsp_scratch
    add     r15, rax                ; r15 = __percpu_rsp_scratch + (__percpu_offset * cpu_id)

    ; 保存rsp到rbp和percpu_ptr(rsp_scratch)
    mov     rbp, rsp
    mov     qword [r15], rsp

    ; 首先判断被中断的是用户态还是内核态
    test    word [rsp+160], 3       ; by checking SS selector's RPL
    jz      .from_kernel

    ; TODO: 如果是用户态，则保存浮点寄存器

    ; 如果是用户态，则切换到内核栈
    mov     rax, qword [rsp - 8]
    mov     rsp, rax

.from_kernel:
    cld

    ; save frame pointer to rbp
    ; mov    rbp, rsp

    ; Call the interrupt handler.
    mov     rdi, %1
    mov     rsi, rbp
    mov     rax, qword int_handler_table
    call    [rax + 8 * %1]

    ; restore saved frame pointer from rbp
    mov     rsp, rbp
    
    ; 切换到目标任务的rsp
    mov     rsp, qword [r15]
    
    ; restore the saved registers.
    restore_regs

    ; skip error code
    add     rsp, 8

    iretq
%endmacro


; internal exceptions, well-defined
define_isr      0
define_isr      1
define_isr      2
define_isr      3
define_isr      4
define_isr      5
define_isr      6
define_isr      7
define_isr_e    8
define_isr      9
define_isr_e    10
define_isr_e    11
define_isr_e    12
define_isr_e    13
define_isr_e    14
define_isr      15
define_isr      16
define_isr_e    17
define_isr      18
define_isr      19
define_isr      20
define_isr      21
define_isr      22
define_isr      23
define_isr      24
define_isr      25
define_isr      26
define_isr      27
define_isr      28
define_isr      29
define_isr      30
define_isr      31

; other not defined interrupts, for later use
define_isr 32
define_isr 33
define_isr 34
define_isr 35
define_isr 36
define_isr 37
define_isr 38
define_isr 39
define_isr 40
define_isr 41
define_isr 42
define_isr 43
define_isr 44
define_isr 45
define_isr 46
define_isr 47
define_isr 48
define_isr 49
define_isr 50
define_isr 51
define_isr 52
define_isr 53
define_isr 54
define_isr 55
define_isr 56
define_isr 57
define_isr 58
define_isr 59
define_isr 60
define_isr 61
define_isr 62
define_isr 63
define_isr 64
define_isr 65
define_isr 66
define_isr 67
define_isr 68
define_isr 69
define_isr 70
define_isr 71
define_isr 72
define_isr 73
define_isr 74
define_isr 75
define_isr 76
define_isr 77
define_isr 78
define_isr 79
define_isr 80
define_isr 81
define_isr 82
define_isr 83
define_isr 84
define_isr 85
define_isr 86
define_isr 87
define_isr 88
define_isr 89
define_isr 90
define_isr 91
define_isr 92
define_isr 93
define_isr 94
define_isr 95
define_isr 96
define_isr 97
define_isr 98
define_isr 99
define_isr 100
define_isr 101
define_isr 102
define_isr 103
define_isr 104
define_isr 105
define_isr 106
define_isr 107
define_isr 108
define_isr 109
define_isr 110
define_isr 111
define_isr 112
define_isr 113
define_isr 114
define_isr 115
define_isr 116
define_isr 117
define_isr 118
define_isr 119
define_isr 120
define_isr 121
define_isr 122
define_isr 123
define_isr 124
define_isr 125
define_isr 126
define_isr 127

define_isr 128
define_isr 129
define_isr 130
define_isr 131
define_isr 132
define_isr 133
define_isr 134
define_isr 135
define_isr 136
define_isr 137
define_isr 138
define_isr 139
define_isr 140
define_isr 141
define_isr 142
define_isr 143
define_isr 144
define_isr 145
define_isr 146
define_isr 147
define_isr 148
define_isr 149
define_isr 150
define_isr 151
define_isr 152
define_isr 153
define_isr 154
define_isr 155
define_isr 156
define_isr 157
define_isr 158
define_isr 159
define_isr 160
define_isr 161
define_isr 162
define_isr 163
define_isr 164
define_isr 165
define_isr 166
define_isr 167
define_isr 168
define_isr 169
define_isr 170
define_isr 171
define_isr 172
define_isr 173
define_isr 174
define_isr 175
define_isr 176
define_isr 177
define_isr 178
define_isr 179
define_isr 180
define_isr 181
define_isr 182
define_isr 183
define_isr 184
define_isr 185
define_isr 186
define_isr 187
define_isr 188
define_isr 189
define_isr 190
define_isr 191
define_isr 192
define_isr 193
define_isr 194
define_isr 195
define_isr 196
define_isr 197
define_isr 198
define_isr 199
define_isr 200
define_isr 201
define_isr 202
define_isr 203
define_isr 204
define_isr 205
define_isr 206
define_isr 207
define_isr 208
define_isr 209
define_isr 210
define_isr 211
define_isr 212
define_isr 213
define_isr 214
define_isr 215
define_isr 216
define_isr 217
define_isr 218
define_isr 219
define_isr 220
define_isr 221
define_isr 222
define_isr 223
define_isr 224
define_isr 225
define_isr 226
define_isr 227
define_isr 228
define_isr 229
define_isr 230
define_isr 231
define_isr 232
define_isr 233
define_isr 234
define_isr 235
define_isr 236
define_isr 237
define_isr 238
define_isr 239
define_isr 240
define_isr 241
define_isr 242
define_isr 243
define_isr 244
define_isr 245
define_isr 246
define_isr 247
define_isr 248
define_isr 249
define_isr 250
define_isr 251
define_isr 252
define_isr 253
define_isr 254
define_isr 255