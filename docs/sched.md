终于，到了讨论调度话题的时候了。

### 计时

但是，在讨论调度之前，有必要明确一下定时功能。抢占式操作系统的调度程序依赖时钟中断，在中断处理函数中进行调度，切换进程。

多核环境下，调度就复杂了。首先，多核的时钟可以用全局的 HPET 或者 PIT，也可以用每个核心局部的 local APIC timer。

使用全局还是局部的时钟取决于调度策略的设计。

如果使用全局的时钟，那么 local APIC timer 还有什么用处？似乎在 Linux 上，APIC timer 用于多核环境下的调度。

### 多核调度

一个简单的方法是每个核心都有自己的调度程序，创建线程的时候为其指定一个核心，之后便只能被这个核心调度。Core局部性有利于提高缓存命中率。

https://web.archive.org/web/http://irqbalance.org/documentation.html
http://linux.linti.unlp.edu.ar/images/e/ec/ULK3-CAPITULO6-UNNOBA.pdf

---

时钟功能体现在两个方面上：
- 获取当前时间、日期的能力
- 计时功能，一段时间过去后回调
