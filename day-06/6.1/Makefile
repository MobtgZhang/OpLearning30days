#Makefile

CC = gcc
CFLAGS = -nostdlib -m32
INCLUDE = -I include

LD = ld
LFLAGS = -m elf_i386

QEMU = qemu-system-x86_64

TARGET_DIR = bin
LST_DIR = $(TARGET_DIR)/lst
TMP_DIR = $(TARGET_DIR)/tmp

OS_SRC_DIR = src
OS_SRC=$(wildcard $(OS_SRC_DIR)/*.c)
OS_LS = scripts/bootpack.lds
OS = $(TARGET_DIR)/os.bin

OS_ENTRY_POINT = boot_main

SYSTEM_IMG = bin/haribote.bin

ASMLIB_SRC = src/asm/asm_func.s
ASMLIB = $(TARGET_DIR)/asm_func.o

BINLIB = $(TARGET_DIR)/fonts.o
SRCLIB = lib/fonts.S

IPL_SRC = src/asm/boot/ipl.S
IPL_LS = scripts/ipl.lds
IPL = $(TARGET_DIR)/ipl.bin

OSL_SRC = src/asm/boot/boot.S
OSL_LS = scripts/boot.lds
OSL = $(TARGET_DIR)/boot.bin

IMG = $(TARGET_DIR)/os.img

all: $(IMG)

$(IMG): $(IPL) $(OSL) $(OS)
	cat $(OSL) $(OS) > $(SYSTEM_IMG)
	mformat -f 1440 -B $(IPL) -C -i $(IMG) ::
	mcopy $(SYSTEM_IMG) -i $(IMG) ::
$(BINLIB): $(SRCLIB)
	$(CC) $(CFLAGS) -c -g -Wa,-a,-ad $< -o $@ > $(addprefix $(LST_DIR)/, $(notdir $(@F:.o=.lst)))

$(OS): $(addprefix $(TARGET_DIR)/, $(notdir $(OS_SRC:.c=.o))) $(ASMLIB) $(BINLIB)
	ld $(LFLAGS) -o $@ -T $(OS_LS) -e $(OS_ENTRY_POINT) --oformat=binary $^

$(ASMLIB): $(ASMLIB_SRC)
	$(CC) $(CFLAGS) -c -g -Wa,-a,-ad $< -o $@ > $(addprefix $(LST_DIR)/, $(notdir $(@F:.o=.lst)))

$(TARGET_DIR)/%.o : $(OS_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -nostdlib -m32 -c -o $@ $<

$(IPL): $(IPL_SRC)
	mkdir -p $(TARGET_DIR)
	mkdir -p $(LST_DIR)
	mkdir -p $(TMP_DIR)
	gcc $(CFLAGS) -o $@ -T$(IPL_LS) $(IPL_SRC)

$(OSL): $(OSL_SRC)
	$(CC) $(CFLAGS) -o $@ -T $(OSL_LS) $(OSL_SRC)
	$(CC) $(CFLAGS) -o $(addprefix $(TMP_DIR)/, $(notdir $(@F:.s=.o))) -T $(OSL_LS) -c -g -Wa,-a,-ad $(OSL_SRC) > $(addprefix $(LST_DIR)/, $(notdir $(@F:.bin=.lst)))

run: all
	$(QEMU) -m 32 -drive format=raw,file=$(IMG),if=floppy

debug: all
	$(QEMU) -drive format=raw,file=$(TARGET_IMG),if=floppy -gdb tcp::10000 -S

clean:
	rm -rf $(TARGET_DIR)

