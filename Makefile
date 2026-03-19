# UEFI x86-64 操作系统构建
# 用法: make [BUILD=Release|Debug] 默认 Release
# 输出目录: build/Release 或 build/Debug

BUILD   ?= Release
BUILD_DIR := build/$(BUILD)
SRC_DIR := src
EFI_DIR := $(SRC_DIR)/efi
UEFI_HDR := $(EFI_DIR)/efi.h
SCRIPTS := scripts

CC      := gcc
AS      := gcc
LD      := ld
OBJCOPY := objcopy

# 调试版本：打开步骤日志
ifeq ($(BUILD),Debug)
  CFLAGS_EXTRA := -DDEBUG_BUILD -O0 -g
else
  CFLAGS_EXTRA := -O2
endif

CFLAGS  := -m64 -mabi=ms -ffreestanding -fno-pie -fno-stack-protector \
           -mno-red-zone -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables \
           -Wall -Wextra -I$(SRC_DIR) $(CFLAGS_EXTRA)
ASFLAGS := -m64 -x assembler-with-cpp -c
LDFLAGS := -nostdlib -static -T $(SCRIPTS)/link.ld -z max-page-size=0x1000

CSOURCES := $(SRC_DIR)/main.c $(SRC_DIR)/serial.c $(SRC_DIR)/log.c $(SRC_DIR)/gop.c \
            $(SRC_DIR)/mouse.c $(SRC_DIR)/haribote_gfx.c $(SRC_DIR)/sheet.c
ASMSRC   := $(SRC_DIR)/boot.S
OBJS     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CSOURCES)) \
            $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%.o,$(ASMSRC))
ELF      := $(BUILD_DIR)/bootx64.elf
EFI      := $(BUILD_DIR)/bootx64.efi
IMG      := $(BUILD_DIR)/uefi-os.img

.DEFAULT_GOAL := all

.PHONY: all clean dirs run img qemu efi

# 默认：编译 + 生成可启动磁盘镜像
all: dirs $(IMG)

dirs:
	@mkdir -p $(BUILD_DIR)

# 仅编译 EFI，不打包镜像
efi: dirs $(EFI)

$(IMG): $(EFI)
	@chmod +x $(SCRIPTS)/mkimg.sh 2>/dev/null || true
	$(SCRIPTS)/mkimg.sh $(BUILD)
	@echo "  IMG $@"

img: $(IMG)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(UEFI_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "  CC  $<"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(AS) $(ASFLAGS) -o $@ $<
	@echo "  AS  $<"

$(OBJS): | dirs

$(ELF): $(OBJS) $(SCRIPTS)/link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "  LD  $@"

$(EFI): $(ELF)
	$(OBJCOPY) -O efi-app-x86_64 --image-base=0x200000 $< $@
	@echo "  EFI $@"

clean:
	rm -rf build

# 用 QEMU 加载 uefi-os.img 运行（需 OVMF）
qemu: $(IMG)
	@chmod +x run-qemu.sh 2>/dev/null || true
	./run-qemu.sh $(BUILD)

run: qemu
