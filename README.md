Wheel
=====

Wheel is an operating system written completely from scratch. Currently Wheel supports 64-bit Intel/AMD CPU.

### Compile and Run

Wheel can be built on most Linux distro. The following tools are required:

- Clang or GCC as the C compiler, Wheel prefer Clang over GCC
- Yasm as the assembler
- GNU Ld as the linker
- MTools for building the floppy image
- QEMU to run the OS
- MkDocs for generating documents

To build the kernel, just run `make kernel`, and the kernel binary image will be generated under directory `build`.

To write the kernel binary to floppy image, run the command `make write`.

To run the OS inside QEMU, just run `make run`.

You can also build, write and run the OS with one command `make`.

Run `make clean` to clean the compile cache.

### Document

Documents can be built using command `mkdocs build`, the generated doc is under `build/site`.
