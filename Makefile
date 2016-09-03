# Makefile for Wheel Operating System

# temperory build directory
DIR	:=	$(PWD)/build

# files
SRC	:=	$(patsubst ./%,%,$(shell find . -name '*.asm' -o -name '*.c'))
HDR	:=	$(patsubst ./%,%,$(shell find . -name '*.h'))
OBJ	:=	$(patsubst %,$(DIR)/%.o,$(SRC))
BIN	:=	$(DIR)/kernel.bin
MAP	:=	$(DIR)/kernel.map
ISO	:=	cd.iso

# toolchain
CC	:=	clang
AS	:=	yasm
LD	:=	ld

# C compiler flags -- basic
CFLAGS	:=	-c -m64 -std=c99 -I include -I include/drivers/acpi -ffreestanding -nostdlib -fno-omit-frame-pointer -fno-stack-protector -fno-zero-initialized-in-bss

# C compiler flags -- warnings
CFLAGS	+=	-Wall -Wextra -Werror-implicit-function-declaration -Wwrite-strings -Wno-unused-function -Wno-unused-parameter -Wno-format

# C compiler flags -- x86-64 only
CFLAGS	+=	-mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow

.PHONY:	iso run clean

iso:	$(BIN)
	cp -f $(BIN) iso/boot/kernel.bin
	grub-mkrescue -o $(ISO) iso

run:	iso
	qemu-system-x86_64 -m 64 -smp 4 -cdrom $(ISO)

clean:
	rm $(OBJ)
	rm $(BIN) $(MAP)

$(BIN):	$(OBJ) kernel.lds
	ld -T kernel.lds -nostdlib -z max-page-size=0x1000 -Map $(MAP) -o $@ $(OBJ)

$(DIR)/%.asm.o:	%.asm
	mkdir -p $(@D)
	$(AS) -f elf64 -o $@ $<

$(DIR)/%.c.o:	%.c $(DIR)/%.d
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -MF $(DIR)/$(basename $<).t -o $@ $<
	mv -f $(DIR)/$(basename $<).t $(DIR)/$(basename $<).d

$(BUILDDIR)/%.d: ;

-include $(patsubst %,$(BUILDDIR)/%.d,$(basename $(SRC)))

