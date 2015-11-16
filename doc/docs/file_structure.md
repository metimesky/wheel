# Wheel 代码文件组织结构

OS 结构很复杂，将代码组织成有序结构需要费一番脑筋。

一般来说，OS 项目的代码结构和 OS 的设计紧密相关。Wheel 设计目标是一个宏内核（可以加入内核模块的功能），并且还要配带相关用户层软件（userland）。因此直观上应该将工程分为 `kernel` 和 `user` 两个目录，分别存放内核与用户程序的代码。

FreeBSD 的源码与此不同，其结构更像是安装好的系统的目录结构，内核放在 `sys` 下，其他软件根据类型不同分散在各个目录中。

Wheel 在开发过程中，首先编写内核部分，之后编写用户软件，因此文件结构可能经常改变。虽然 Wheel 大体上遵循宏内核设计，但是代码组织同样可以模块化。一些模块化编程习惯包括区分公有和私有头文件，

### 预计的结构

```
wheel（kernel部分）
├── include         # 这里存放内核使用的全局头文件（或者命名为 common）
│   ├── stdhdr.h
│   ├── multiboot.h
│   └── util.h      # 所有 library 中定义的函数（或者叫做 systen.h、wheel.h？）
├── init            # 内核并没有主循环，这部分存放启动代码
│   ├── boot.asm    # 从 GRUB 获取控制权，切换 64 位方式
│   ├── init.c      # init 函数，相当于内核的 main 函数，实际上就是启动过程
├── memory          # 内存管理部分
│   ├── memory.c    # 实现
│   ├── memory.h    # 接口
│   ├── page_alloc.c# 物理内存管理实现
│   ├── page_alloc.h# 物理内存管理接口
│   ├── virt_alloc.c# 虚拟内存管理实现
│   ├── virt_alloc.h# 虚拟内存管理接口
│   ├── slab_alloc.c# 对象内存管理实现
│   └── slab_alloc.h# 对象内存管理接口
├── interrupt       # 中断处理子模块
│   ├── interrupt.c # 实现
│   ├── interrupt.h # 接口
│   ├── entries.asm # 中断处理函数入口（汇编实现）
│   └── entries.h   # 物理内存管理接口
├── library         # 有点类似与 C 标准库，但是仅供内核使用（工具函数、数据结构等）
│   ├── string.c    # 字符串相关函数、memcpy、memset 等
│   ├── string.asm  # 汇编实现的 string 函数，用于加速
│   ├── cpu.asm     # 一些汇编实现的函数
│   ├── cpu.h       # 一些内联汇编的函数（包括 cpu.asm 中的函数的原型）
│   ├── bitmap.c    # 位图数据结构的实现
│   └── bitmap.h    # 位图数据结构的接口
├── acpi            #
│   ├── acpi.h      #
│   ├── acpi.c      #
│   ├── acpicaos.c  # Wheel 提供的 ACPICA 接口层
│   ├── acpica/     # intel 的开源项目，拿来直接用
.....
```

关于 `include` 目录，某些项目把所有的头文件都放在这里。我更倾向于将同一个编译单元的头文件和 C 文件放在一起。然而，某些头文件没有对应的 C 文件，或者某些头文件就是对其他头文件的引用。
