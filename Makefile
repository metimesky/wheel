# Makefile for SOS

# directories
source_dir	:=	boot kernel
include_dir	:=	include
build_dir	:=	build

# files
sources	:=	$(foreach dir, $(source_dir), $(shell find $(dir) -name '*.asm' -o -name '*.c'))
headers	:=	$(foreach dir, $(include_dir), $(shell find $(dir) -name '*.h'))
objects	:=	$(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(build_dir)/$(obj))
kernel	:=	$(build_dir)/kernel.bin
floppy	:=	fd.img

# tool chain
AS		:=	yasm
AFLAGS	:=	-f elf
CC		:=	clang
CFLAGS	:=	-c -m32 -std=c99 -I $(include_dir) \
			-ffreestanding -fno-builtin -nostdinc -nostdlib \
			-O2 -Wall -Wextra
LD		:=	ld
LFLAGS	:=	-m elf_i386 -T link.lds

.PHONY	:=	all bin write run clean

all:		bin write run

bin:		$(kernel)

write:		$(kernel) $(floppy)
	@echo "\033[1;34mwriting to floppy image\033[0m"
	@mcopy -o $(kernel) -i $(floppy) ::/

run:		$(floppy)
	@echo "\033[1;31mexecuting qemu\033[0m"
	@qemu-system-x86_64 -m 32 -smp 2 -fda $(floppy)

clean:
	@echo "\033[1;34mcleaning objects\033[0m"
	@rm $(objects) $(kernel)

$(kernel):	$(objects) link.lds
	@echo "\033[1;34mlinking kernel\033[0m"
	@mkdir -p $(@D)
	@$(LD) $(LFLAGS) -o $@ $^

$(build_dir)/%.asm.o:	%.asm
	@echo "\033[1;32massembling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(AS) $(AFLAGS) -o $@ $<

$(build_dir)/%.c.o:	%.c $(headers)
	@echo "\033[1;32mcompiling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

