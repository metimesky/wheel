Wheel
=====

Wheel is an operating system written from scratch. Currently Wheel is only compatible with AMD64 CPU.

### Code Structure

OS is complex with its component link to each other, so it's hard to organize the source code cleanly.

maybe we can make a directory called `basic`, containing low level stuff like interrupt entry. or we can simple call that directory `kernel`

### Dev

Wheel is written in Assembly and C, developed using YASM and Clang under Linux environment. GNU-LD is used as linker, MTools is used to read/write floppy dist image. Bochs and QEMU are employed for testing.

You can use the following command to install all the programs needed:

~~~
sudo apt-get install build-essential yasm clang mtools qemu-kvm
~~~

### Starting procedure

GRUB handles control to `init/boot.asm`, it then execute `init/stage0.asm`, which checks the long mode, setup stack, paging and temporary GDT. then jumps into `init/stage1.c`, which setup the real GDT and IDT(WIP).