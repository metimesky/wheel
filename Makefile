# Makefile for Wheel Operating System

# Requirements:
#   yasm, clang, binutils, mtools, qemu
# TODO:
# - generate floopy image rather than using a pre-built one

################################################################################

# directories
inc_dir :=  include
src_dir :=  kernel acpi memory library
dst_dir :=  build

# files
sources :=  $(foreach dir, $(src_dir), $(shell find $(dir) -name '*.asm' -o -name '*.c'))
headers :=  $(foreach dir, $(src_dir) $(inc_dir), $(shell find $(dir) -name '*.h'))
objects :=  $(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(dst_dir)/$(obj))

bin     :=  $(dst_dir)/kernel.bin
map     :=  $(dst_dir)/kernel.map
lds     :=  link.lds
fda     :=  fd.img

# toolchain
AS      :=  yasm
ASFLAGS :=  -f elf64
CC      :=  clang
CFLAGS  :=  -c -std=c11 -O2 -Wall -Wextra -I $(inc_dir) -ffreestanding -fno-builtin \
            -fno-stack-protector -fno-zero-initialized-in-bss -fno-sanitize=address \
            -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
            # `-ffreestanding` implies `-fno-builtin`, and `-nostdlib` is used in linking
LD      :=  ld
LDFLAGS :=  -nostdlib -z max-page-size=0x1000

################################################################################

# pseudo-targets
.PHONY: all kernel write run clean

all: kernel write run

kernel: $(bin)

write: $(bin) $(fda)
	@echo "\033[1;34mwriting to floppy image\033[0m"
	@mcopy -o $(bin) -i $(fda) ::/

run: $(fda)
	@echo "\033[1;31mexecuting qemu\033[0m"
	@qemu-system-x86_64 -m 32 -smp 2 -fda $(fda)

clean:
	@echo "\033[1;34mcleaning objects\033[0m"
	@rm $(objects) $(bin)

# NOTE: special rules first, generic rules later
# make will use first rule matching the pattern

$(bin): $(objects) $(lds)
	@echo "\033[1;34mlinking kernel\033[0m"
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -T $(lds) -Map $(map) -o $@ $^

$(dst_dir)/%.asm.o: %.asm
	@echo "\033[1;32massembling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) -o $@ $<

$(dst_dir)/%.c.o: %.c $(headers)
	@echo "\033[1;32mcompiling $< to $@\033[0m"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<
