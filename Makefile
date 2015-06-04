# Makefile for Wheel Operating System

# directories
source_dir  :=  i386 kernel libk
include_dir :=  include
build_dir   :=  build

# files
sources :=  $(foreach dir, $(source_dir), $(shell find $(dir) -name '*.asm' -o -name '*.c'))
headers :=  $(foreach dir, $(include_dir), $(shell find $(dir) -name '*.h'))
objects :=  $(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(build_dir)/$(obj))
kernel  :=  $(build_dir)/kernel.bin
floppy  :=  fd.img

# 32-bit
AS      :=  yasm
AFLAGS  :=  -f elf
CC      :=  gcc
CFLAGS  :=  -c -m32 -std=c99 -I $(include_dir) \
            -ffreestanding -fno-builtin -nostdinc -nostdlib \
            -Wall -Wextra
LD      :=  ld
LFLAGS  :=  -m elf_i386 -T link.lds

# 64-bit
AS      :=  yasm
AF      :=  -f elf64
CC      :=  clang
CF      :=  -c -std=c99 -I $(include_dir) \
            -ffreestanding -fno-builtin -nostdinc -nostdlib \
            -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
            -fno-sanitize=address -Wall -Wextra
LD      :=  ld
LF      :=  -T link.lds -z max-page-size=0x1000