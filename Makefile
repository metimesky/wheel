# Makefile for Wheel Operating System

# directories
source_dir  :=  boot kernel memory library
include_dir :=  include
build_dir   :=  build

# files
sources :=  $(foreach dir, $(source_dir), $(shell find $(dir) -name '*.asm' -o -name '*.c'))
headers :=  $(foreach dir, $(include_dir), $(shell find $(dir) -name '*.h'))
objects :=  $(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(build_dir)/$(obj))
kernel  :=  $(build_dir)/kernel.bin
floppy  :=  fd.img

# toolchain
AS      :=  yasm
AF      :=  -f elf64
CC      :=  clang
CF      :=  -c -std=c99 -I $(include_dir) \
            -ffreestanding -fno-builtin -nostdlib \
            -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
            -fno-sanitize=address -Wall -Wextra
LD      :=  ld
LF      :=  -T link.lds -z max-page-size=0x1000 -Map wheel.map

# pseudo-targets
.PHONY: all bin write run clean

all: bin write run

bin: $(kernel)

write: $(kernel) $(floppy)
	@echo "\033[1;34mwriting to floppy image\033[0m"
	@mcopy -o $(kernel) -i $(floppy) ::/

run: $(floppy)
	@echo "\033[1;31mexecuting qemu\033[0m"
	@qemu-system-x86_64 -m 32 -smp 2 -fda $(floppy)

clean:
	@echo "\033[1;34mcleaning objects\033[0m"
	@rm $(objects) $(kernel)

$(kernel):  $(objects) link.lds
	@echo "\033[1;34mlinking kernel\033[0m"
	@mkdir -p $(@D)
	@$(LD) $(LF) -o $@ $^

$(build_dir)/%.asm.o: %.asm
	@echo "\033[1;32massembling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(AS) $(AF) -o $@ $<

$(build_dir)/%.c.o: %.c $(headers)
	@echo "\033[1;32mcompiling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(CC) $(CF) -o $@ $<
