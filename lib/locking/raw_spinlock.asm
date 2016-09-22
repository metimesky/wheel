; spinlock_t是32位整数
; BTS指令会将指定位的值保存在CF中，然后置1

global raw_spin_lock
global raw_spin_trylock
global raw_spin_unlock
global raw_spin_islocked

; 获得自旋锁的同时，还禁用抢占，保证spin_lock和spin_unlock之间的临界区不被打断。
global spin_lock

; 定义在scheduler模块中，表示是否启用抢占
extern preempt_enable
extern preempt_disable

[section .text]
[BITS 64]

; void raw_spin_lock(raw_spinlock_t *lock);
raw_spin_lock:
    lock
    bts     dword [rdi], 0      ; 最低位置1，原值保存在CF中
    jc      .spin_with_pause    ; 若CF为1，说明已经被其他线程获取
    ret
 
.spin_with_pause:
    pause                       ; relax cpu
    test    dword [rdi], 1      ; Is the lock free?
    jnz     .spin_with_pause    ; no, wait
    jmp     raw_spin_lock       ; retry

; bool raw_spin_trylock(raw_spinlock_t *lock);
raw_spin_trylock:
    lock
    bts     dword [rdi], 0
    jc      .fail
    mov     rax, 1
    ret
.fail:
    mov     rax, 0
    ret

; void raw_spin_unlock(raw_spinlock_t *lock);
raw_spin_unlock:
    mov     dword [rdi], 0
    ret

; bool raw_spin_islocked(raw_spinlock_t *lock);
raw_spin_islocked:
    cmp     dword [rdi], 0
    je      .free
    jne     .locked
.free:
    mov     rax, 0
    ret
.locked:
    mov     rax, 1
    ret
