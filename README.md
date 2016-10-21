Wheel Dev Log
=============

## 参考项目

- [ReturnInfinity/BareMetal-OS](https://github.com/ReturnInfinity/BareMetal-OS)
- [cloudius-systems/osv](https://github.com/cloudius-systems/osv)
- [charliesome/jsos](https://github.com/charliesome/jsos)

然而，上述三个系统似乎都没有提供SMP的支持。

## 交叉编译器

> 用Clang编译OS可能更有吸引力，然而GCC更为可靠且被更多地项目所使用。

要编译Wheel，首先需要准备x86_64-elf目标的交叉编译工具链。当前使用的GCC版本为6.2.0，binutils版本为2.27。

在Linux环境下编译binutils和GCC，需要安装automake、autoconf、texinfo、libtool、flex、bison：
`sudo apt-get install automake autoconf texinfo libtool flex bison`。

配置和编译安装的步骤如下：

``` bash
mkdir -pv tools && cd tools

wget ftp://ftp.gnu.org/gnu/binutils/binutils-2.27.tar.gz
tar vxf binutils-2.27.tar.gz
cd binutils-2.27
mkdir build && cd build
../configure --prefix=/mnt/c/Users/admin/workSpace/wheel/tools --target=x86_64-elf --with-sysroot --disable-nls --disable-werror
make -j4
make install

wget ftp://ftp.gnu.org/gnu/gcc/gcc-6.2.0/gcc-6.2.0.tar.gz
tar vxf gcc-6.2.0.tar.gz
cd gcc-6.2.0
contrib/download_prerequisites
mkdir build && cd build
../configure --prefix=/mnt/c/Users/admin/workSpace/wheel/tools --target=x86_64-elf --enable-languages=c --disable-nls --without-headers
export PATH=/mnt/c/Users/admin/workSpace/wheel/tools/bin:$PATH
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

这里对于GCC，仅仅开启了C编译器，其他语言和C++均不开启。

编译GCC的命令不是简单地make，编译方法需要查一下文档进行确认。
此外，编译GCC的时候，需要设置PATH环境变量，这样才能使用之前编译好的binutils。