# Makefile for Wheel Operating System

# directories
src_dir :=  kernel
dst_dir :=  build

# files
sources :=  $(foreach dir, $(src_dir), $(shell find $(dir) -name '*.asm' -o -name '*.c'))
headers :=  $(foreach dir, $(src_dir), $(shell find $(dir) -name '*.h'))
objects :=  $(foreach obj, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.c.o, $(sources))), $(dst_dir)/$(obj))

# scripts and output files
lds     :=  link.lds
bin     :=  $(dst_dir)/kernel.bin
map     :=  $(dst_dir)/kernel.map
fda     :=  fd.img

# toolchain
AS      :=  yasm
ASFLAGS :=  -f elf64
CC      :=  $(if $(shell which clang), clang, gcc)
CFLAGS  :=  -std=c11 -I $(src_dir) -ffreestanding \
            -Wall -Wextra -Werror-implicit-function-declaration \
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
	@printf "\033[1;34mwriting to floppy image\033[0m\n"
	@mcopy -o $(bin) -i $(fda) ::/

run: $(fda)
	@printf "\033[1;31mexecuting qemu\033[0m\n"
	@qemu-system-x86_64 -m 32 -smp 2 -fda $(fda) -d guest_errors,int -D log.txt

clean:
	@printf "\033[1;34mcleaning objects\033[0m\n"
	@rm $(objects) $(bin)

$(bin): $(objects) $(lds)
	@printf "\033[1;34mlinking kernel\033[0m\n"
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -T $(lds) -Map $(map) -o $@.elf $^
	@objcopy -O binary $@.elf $@

$(dst_dir)/%.asm.o: %.asm
	@printf "\033[1;32massembling $< to $@\033[0m\n"
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) -o $@ $<

$(dst_dir)/%.c.o: %.c $(dst_dir)/%.d
	@printf "\033[1;32mcompiling $< to $@\033[0m\n"
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF $(dst_dir)/$*.td -o $@ $<
	@mv -f $(dst_dir)/$*.td $(dst_dir)/$*.d # avoid corrupted dep file due to compile error

# create a pattern rule with empty recipe, so make won't fail if
# dependecy file doesn't exist
$(dst_dir)/%.d: ;

# include the dependency files that exist
# use `-include` to avoid failing on non-exist files
-include $(patsubst %, $(dst_dir)/%.d, $(basename $(sources)))