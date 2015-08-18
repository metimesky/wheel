# Makefile for Wheel Operating System

# From `http://clang.llvm.org/docs/CrossCompilation.html`:
# - Clang/LLVM is natively a cross-compiler

# directories
source_dir  :=  boot include
build_dir   :=  build

# files
assembs	:=	$(foreach dir, $(source_dir), $(shell find $(dir) -name '*.asm'))
sources :=  $(foreach dir, $(source_dir), $(shell find $(dir) -name '*.c'))
headers :=  $(foreach dir, $(source_dir), $(shell find $(dir) -name '*.h'))
objects :=  $(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(build_dir)/$(obj))
kernel  :=  $(build_dir)/kernel.bin
mapfile :=  $(build_dir)/wheel.map
floppy  :=  fd.img

# toolchain
AS      :=  yasm
AF      :=  -f elf64
CC      :=  clang
CFLAGS  :=  -c -std=c11 -I $(include_dir) \
			-ffreestanding -fno-builtin -nostdlib -Wall -Wextra
CF32    :=  $(CFLAGS) -m32
CF64    :=  c -std=c99 -I $(include_dir) \
			-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -fno-sanitize=address \
			-mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
LD      :=  ld
LF      :=  -T link.lds -z max-page-size=0x1000

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
	@$(LD) $(LF) -Map $(mapfile) -o $@ $^

$(build_dir)/%.asm.o: %.asm
	@echo "\033[1;32massembling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(AS) $(AF) -o $@ $<

# pattern matching will match this rule first
$(build_dir)/setup32.c.o: boot/setup.c $(headers)
	@echo "\033[1;32mcompiling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(CC) $(CF32) -o $@ $<

$(build_dir)/%.c.o: %.c $(headers)
	@echo "\033[1;32mcompiling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(CC) $(CF64) -o $@ $<
