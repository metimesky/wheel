# Wheel

Wheel 是一个非常简单的单用户单处理器操作系统, 其存在的目的就是为了重新发明操作系统这个轮子, 并学习操作系统的概念和原理.

Wheel 能运行在 IA-32 体系架构的 CPU 上, 工作在 32 位保护模式下, 可以使用裸机或 QEMU 等虚拟化软件运行.

### 编译运行

Wheel 使用汇编语言和 C 语言编写, 在 Linux 下开发. 编译器依赖 YASM 和 GCC, 链接器使用 LD, 创建可引导软盘镜像使用 MTools, 如果需要在虚拟机中运行, 还需要安装 QEMU. 在 Ubuntu/Debian 系统下, 可以使用下面的命令安装依赖项:

~~~
sudo apt-get install build-essential yasm mtools qemu-kvm
~~~

Wheel 使用 make 进行编译链接操作, make 命令会创建 build 目录, 并将生成的目标文件以及内核镜像置于其中, 并使用 MTools 将内核镜像拷贝到软盘镜像中.

### Multiboot

Multiboot is the spec Wheel follow. With multiboot, Wheel can be booted using grub.