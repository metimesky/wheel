; spinlock_t是32位整数

global spinlock_lock
global spinlock_free

[section .text]
[BITS 64]

; void spinlock_lock(spinlock_t *lock);
spinlock_lock:
    lock
    bts     dword [rdi], 0      ;Attempt to acquire the lock (in case lock is uncontended)
    jc      .spin_with_pause
    ret
 
.spin_with_pause:
    pause                       ; Tell CPU we're spinning
    test    dword [rdi], 1      ; Is the lock free?
    jnz     .spin_with_pause    ; no, wait
    jmp     spinlock_lock       ; retry

; void spinlock_free(spinlock_t *lock);
spinlock_free:
    mov dword [rdi], 0
    ret