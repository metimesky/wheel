; 获得自旋锁的同时还会禁用抢占
global spin_lock
global spin_trylock
global spin_unlock

; 定义在scheduler模块中，表示是否启用抢占
extern preempt_enable
extern preempt_disable

[section .text]
[BITS 64]

; void spin_lock(raw_spinlock_t *lock);
spin_lock:
    mov     r8, qword preempt_disable
    mov     r9, qword preempt_enable
    call    r8                          ; 禁用抢占 TODO: 应该为per-CPU var
    lock
    bts     dword [rdi], 0              ; 最低位置1，原值保存在CF中
    jc      .lock_fail                  ; 若CF为1，说明目前无法获得锁
    ret                                 ; 成功获得锁，此时抢占仍为禁用状态
 
.lock_fail:
    call    r9                          ; 重新启用抢占
.spin_with_pause:
    pause                               ; Tell CPU we're spinning
    test    dword [rdi], 1              ; Is the lock free?
    jnz     .spin_with_pause            ; no, wait
    jmp     spin_lock                   ; retry

; bool spin_trylock(raw_spinlock_t *lock);
spin_trylock:
    mov     r8, qword preempt_disable
    mov     r9, qword preempt_enable
    call    r8
    lock
    bts     dword [rdi], 0
    jc      .fail
    mov     rax, 1
    ret
.fail:
    call    r9
    mov     rax, 0
    ret

; void spin_unlock(raw_spinlock_t *lock);
spin_unlock:
    mov     r9, qword preempt_enable
    mov     dword [rdi], 0
    call    r9
    ret