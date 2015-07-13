# Wheel

Wheel 是一个非常简单的单用户单处理器操作系统, 其存在的目的就是为了重新发明操作系统这个轮子, 并学习操作系统的概念和原理.

Wheel 能运行在 AMD64 体系架构的 CPU 上, 工作在 64 位长方式下, 可以使用裸机或 QEMU 等虚拟化软件运行.

### 编译运行

Wheel 使用汇编语言和 C 语言编写, 在 Linux 下开发. 编译器依赖 YASM 和 GCC, 链接器使用 LD, 创建可引导软盘镜像使用 MTools, 如果需要在虚拟机中运行, 还需要安装 QEMU. 在 Ubuntu/Debian 系统下, 可以使用下面的命令安装依赖项:

~~~
sudo apt-get install build-essential yasm mtools qemu-kvm
~~~

Wheel 使用 make 进行编译链接操作, make 命令会创建 build 目录, 并将生成的目标文件以及内核镜像置于其中, 并使用 MTools 将内核镜像拷贝到软盘镜像中.

### Multiboot

Multiboot is the spec Wheel follow. With multiboot, Wheel can be booted using grub.

### Portability

Wheel does not care about portability. Thus this os is currently bind tightly with AMD64 arch.

### 长方式之坑

64-bit 限制了分段机制的使用，但是仍然会用到段。而且 64-bit 模式下，段描述符的长度有8字节和16字节两种，例如在GDT中，16字节的描述符相当于占两个8字节描述符的位置。CPU根据偏移访问某描述符时，有些情况下将index乘8，另一些时候乘16。
