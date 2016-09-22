## Compiling Bochs

```
./configure --enable-smp --enable-cpu-level=6 --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx --enable-debugger --enable-disasm --enable-debugger-gui --enable-logging --enable-fpu --enable-3dnow --enable-cdrom --enable-x86-debugger --enable-iodebug --disable-plugins --disable-docbook --with-x --with-x11 --with-term
```

## Per-CPU Data

per-CPU Data Section中只能保存指针类型的数据，此外在per-CPU Section中还定义了内核栈，因此访问各自内核的内核栈，只能单独用__percpu_offset和自己的ID进行计算

Linux似乎将per-CPU Data之间的间隔设置得大一些，因为per-CPU Data不仅有编译时已经定义的静态变量，还有运行时动态申请的部分。动态per-CPU Data按chunk大小增长，chunk是在Linux内核配置时确定的。

## MM

Linux的物理内存管理是分不同zone分割的，每个zone都有自己的spinlock。操作一个zone之内的内存页面时，首先需要获取这个spinlock。

## 多核调度

CPU有超频和超线程的技术，超频能让CPU根据自身温度调节主频，而超线程能够让一个物理核心完成两个逻辑核心的功能。

超频的影响就是，SMP下不同核心的执行速度未必相同，localAPIC的时钟频率也会随之改变。

多核调度策略可以分为三类：全局、分区、部分分区。最根本的区别就是调度进程队列是只有一个还是每个核心都有。分区的策略需要在创建进程的时候决定分配给哪个核心，然后每个核心上分别运行单核的调度算法。这种分区的策略没有进程迁移的机会，因此不会有缓存问题（但是CPU往往有共享的L2Cache），但是可能造成给每个核心的负载分配不均衡。

> Newer multiprocessor architectures, such as multi-core processors, have significantly reduced the migration overhead. The preference for partitioned scheduling may no longer be necessary on these types of multiprocessors

部分分区是一种折衷方案，大部分的任务只在特定的核心上运行，一小部分任务在全局的队列中。

Linux中，有一个per-CPU的结构，称作runqueue。

调度器需要能够禁用抢占（pre-emption），因为进程处在内核态的时候也有可能被中断，因此在获取锁的时候，需要首先禁用抢占，保证接下来操作在当前核心上的原子性，然后通过lock前缀获取raw_spinlock，保证不同核心之间的原子性。

简单自旋锁（raw_spinlock）可以实现多核之间的互斥，但是不能保证上锁到释放锁之间的原子性，因为一个核心内部也会有中断，也会有上下文切换。然而，在访问per-CPU对象的时候，简单自旋锁是完全足够的。

真的需要不被打断的操作吗？本系统没有实时性需求。在ISR中，代码的执行不会被打断，对于进程来说，只要获得资源后不会被其他进程抢去，就不会有问题，毕竟进程切换保存了上下文。

问题在于raw_spin_lock这类函数本身不是原子性的，可能在尝试获取

### 同步控制

[内核层并发控制](http://people.cs.pitt.edu/~ouyang/20150225-kernel-concurreny.html)

如果多个控制流访问了共享的全局数据，就有可能发生竞争的情况。为此，必须用某种控制机制进行约束。对于OS内核来说，竞争有三个原因：
- 多核(SMP)。有了多核之后，可以真正地在硬件层面实现并发执行了。因此在两个核心上运行的内核代码有可能会同时访问相同数据。
- 抢占。如果允许内核态抢占，那么这也会成为竞争的原因之一。假设进程A正在内核态执行，正在访问数据X，此时被进程B抢占，紧接着B也进入内核态执行，同样访问了数据X。即使OS不允许在内核态发生抢占，也可能在ISR中发生竞争，因为中断可以“抢占”内核态代码。
- 中断。凡是被ISR和其他内核控制流共享的数据，都可能因为中断而发生竞争。

有三种竞争来源，同样有对应的机制防止这些竞争情况。
Wheel的简单自旋锁可以避免SMP之间的竞争，因为汇编中的lock前缀可以独占总线。然而并没有防止抢占，因此进程A获得了自旋锁之后，仍有可能被调度器切出，换成进程B。进程B可能也尝试获得该锁，但直到A释放该锁，B并不能继续执行，导致严重的性能下降。

Linux中的自旋锁还能禁用抢占