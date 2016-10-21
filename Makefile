# Makefile for Wheel Operating System
# Wheel构建系统
# 支持在源码目录之外编译

SRC_DIR	:=	$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR	:=	$(shell pwd)

# files
SRC	:=	$(patsubst ./%,%,$(shell find $(SRC_DIR) -name '*.asm' -o -name '*.c'))
HDR	:=	$(patsubst ./%,%,$(shell find $(SRC_DIR) -name '*.h'))
OBJ	:=	$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(SRC))
BIN	:=	$(OBJ_DIR)/kernel.bin
MAP	:=	$(OBJ_DIR)/kernel.map
ISO	:=	cd.iso

# toolchain
TARGET	:=	x86_64-elf
CC	:=	$(TARGET)-gcc
AS	:=	yasm
LD	:=	$(TARGET)-ld

# C compiler flags
CFLAGS	:=	\
	-m64 -std=c99 -Iinclude -nostdlib -nostartfiles -nodefaultlibs -nostdinc -ffreestanding \
	-fno-builtin -fno-omit-frame-pointer -fno-stack-protector -fno-zero-initialized-in-bss \
	-Wall -Wextra -Werror-implicit-function-declaration -Wwrite-strings -Wno-format \
	-mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow

iso:	check $(BIN)
	cp -f $(BIN) $(SRC_DIR)/iso/boot/kernel.bin
	grub-mkrescue -o $(ISO) $(SRC_DIR)/iso

run:	iso
	qemu-system-x86_64 -m 64 -smp 4 -cdrom $(ISO)

check:
ifeq ($(SRC_DIR), $(OBJ_DIR))
	$(error Out-of-source build is required!)
endif

clean:
	rm $(OBJ)
	rm $(BIN) $(MAP)

$(BIN):	$(OBJ) $(SRC_DIR)/kernel.lds
	$(LD) -T $(SRC_DIR)/kernel.lds -nostdlib -z max-page-size=0x1000 -Map $(MAP) -o $@ $(OBJ)

$(OBJ_DIR)/%.asm.o:	$(SRC_DIR)/%.asm
	mkdir -p $(@D)
	$(AS) -f elf64 -o $@ $<

$(OBJ_DIR)/%.c.o:	$(SRC_DIR)/%.c $(OBJ_DIR)/%.d
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -MF $(OBJ_DIR)/%.t -o $@ $<
	mv -f $(OBJ_DIR)/%.t $(OBJ_DIR)/%.d

$(OBJ_DIR)/%.d: ;

-include $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.c.o.d,$(basename $(SRC)))

