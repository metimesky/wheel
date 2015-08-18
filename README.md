Wheel
=====

Wheel is an operating system written from scratch. Currently Wheel is only compatible with AMD64 CPU.

### Dev

Wheel is written in Assembly and C, developed using YASM and Clang under Linux environment. GNU-LD is used as linker, MTools is used to read/write floppy dist image. Bochs and QEMU are employed for testing.

You can use the following command to install all the programs needed:

~~~
sudo apt-get install build-essential yasm clang mtools qemu-kvm
~~~

### Build system

OS is low-level software, so GNU-make is used to automate the building procedure of Wheel.