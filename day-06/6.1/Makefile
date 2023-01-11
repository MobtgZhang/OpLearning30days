CC = gcc
CFLAGS = -nostdlib -m32
INCLUDE = -I include
LD = ld
LFALGS = -m elf_i386

QEMU = qemu-system-x86-64
TARGET_DIR = bin
LST_DIR = $(TARGET_DIR)/lst
TMP_DIR = $(TARGET_DIR)/tmp

OS_SRC_DIR = src
OS_SRC=$(wildcard $(OS_SRC_DIR)/*.c)
OS_LDS=arch/scripts/main.lds
OS_BIN=$(TARGET_DIR)/os.bin

OS_ENTRY_POINT = boot_main

SYSTEM_BIN = $(TARGET_DIR)/system.bin

ASMLIB_SRC = arch/x86/asm_funcs.S
ASMLIB = $(TARGET_DIR)/asm_funcs.o

FONTLIB_SRC = arch/x86/fonts.S
FONTLIB_BIN = $(TARGET_DIR)/fonts.o

IPL_SRC = arch/x86/ipl.S
IPL_LDS = scripts/ipl.lds
IPL_BIN = $(TARGET_DIR)/ipl.bin

OSL_SRC = arch/x86/boot.S
OSL_LDS = scripts/boot.lds
OSL = $(TARGET_DIR)/boot.bin

OS_IMG = $(TARGET_DIR)/os.img

all:$(OS_IMG)


