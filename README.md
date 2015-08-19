Wheel
=====

Wheel is an operating system written from scratch. Currently Wheel is only compatible with AMD64 CPU.

### Dev

Wheel is written in Assembly and C, developed using YASM and Clang under Linux environment. GNU-LD is used as linker, MTools is used to read/write floppy dist image. Bochs and QEMU are employed for testing.

You can use the following command to install all the programs needed:

~~~
sudo apt-get install build-essential yasm clang mtools qemu-kvm
~~~

### Source structure

`init` contains the entry point from grub and assembly code for enter long mode. Since we can't compile some C file as 32-bit and link them together, we can only do 32-bit initializing in assembly. But the 64-bit environment setted by assembly is just temporary. A new full-featured 64-bit environment will be setted up by C code, including a new GDT, new page tables.

### Limitation

Wheel only cares about AMD64 long-mode, so the code is not so portable.