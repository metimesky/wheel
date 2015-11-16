Wheel
=====

Wheel 是一个独立自主实现的操作系统，没有别的目的，就是因为好玩。

Wheel 支持主流 PC 硬件平台，只能够运行在支持 64 位的 Intel CPU 上。

### 编译运行

编译运行 Wheel 操作系统，需要 Linux 环境和以下软件：
- C 语言编译器，Wheel 使用 Clang，也可以更改 `Makefile` 换成 GCC。
- 汇编器，Wheel 使用 Yasm。
- 链接器，Wheel 使用 Linux 下标准的 ld
- MTools，用于构建软盘镜像
- QEMU，用于运行 Wheel 的虚拟机（非必须）
- MkDocs，用于构建文档的 Python 模块（非必须）

编译 Wheel 内核，在命令行下执行 `make kernel`，在 `build` 目录下会生成 `kernel.bin` 文件。

将内核镜像写入虚拟软盘，可以运行命令 `make write`。

使用 QEMU 从软盘镜像启动 Wheel，可以运行命令 `make run`。

使用不带参数的 `make` 命令，会依次执行编译内核、写入软盘镜像、启动 QEMU 虚拟机。

命令 `make clean` 可以清除编译缓存。

### 文档

使用 MkDocs 生成文档，可以使用命令 `mkdocs build`，生成的文档位于 `build/site` 目录下。

执行命令 `mkdocs serve`，可以在浏览器中查看生成的文档。

文档包含了 Wheel 实现过程的许多细节，相当于 Wheel 编写过程中的笔记。

