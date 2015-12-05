# 请页和映射

物理内存管理器管理的是物理内存，向 OS 其他模块提供分配和回收物理页的接口。虚拟内存分配器使用该接口实现 `malloc` 和 `free` 的功能。Wheel 的物理内存管理使用伙伴算法实现。

## 伙伴算法

伙伴算法也是 Linux 用来物理页的算法，基本思想是相邻的块合并，这样可分配的大小都是 2 的整数次幂。

实现伙伴算法的关键是位图数据结构的支持。Wheel 中，通用的工具函数和数据结构/算法的代码放在 `library` 目录下，实现位图的代码在 `bitmap.{c,h}` 中。

Wheel 一共实现了 7 层的 buddy。

## 分页权限

由于物理内存管理器顺带负责建立分页映射，所以也要处理权限。页表项的 U/S 位表示用户是否可以访问这个页。Wheel 的原则是，除了最后一级页表项，其他的表项 U/S 权限都放开，也就是某一个是否允许用户访问全都由最后一级的页表项决定。

## additional thoughts

> 目前实现的物理内存分配是“够用就行”的，但是从论坛上，看到有帖子讨论多线程的情况。一般来说，多线程就是给物理内存分配器加锁，但是这样会影响性能，于是可以使用CPU的`test_and_set`功能。往往检测位图中的0并不是逐个二进制位地检测，而是直接比较`uint64_t`来加速，因此如果要比较特定位，可以用`atomic_and(&block, mask)`来检查。
>
> 一些汇编指令：bts、cmpxchg、lock前缀
>
> 英特尔的关于实现多核锁的文档：https://software.intel.com/en-us/articles/implementing-scalable-atomic-locks-for-multi-core-intel-em64t-and-ia32-architectures。
>
> 首先使用内联汇编的串搜索指令“rep scasq”，当搜到一个包含未分配页的比特之后，执行“lock bts”和“jc found”


