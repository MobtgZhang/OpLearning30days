#!/bin/sh
# 生成可启动的 UEFI 磁盘镜像（GPT + FAT32 ESP，OVMF/q35+IDE 可识别）
# 依赖: dosfstools (mkfs.fat)、mtools (mmd, mcopy)、sgdisk (gptfdisk)
# 说明: 仅「整盘 FAT、无分区表」在多数固件下对固定硬盘不可启动，故使用 GPT。
set -e

BUILD="${1:-Release}"
BUILD_DIR="build/${BUILD}"
OUT="${BUILD_DIR}/uefi-os.img"
EFI="${BUILD_DIR}/bootx64.efi"
SIZE_MB="${IMG_SIZE_MB:-64}"

# ESP 从 LBA 2048 开始（1MiB 对齐，与常见 UEFI 一致）
PART_START_SECTOR=2048
ESP_BYTE_OFF=$((PART_START_SECTOR * 512))

if [ ! -f "$EFI" ]; then
	echo "未找到 $EFI，请先执行: make BUILD=$BUILD"
	exit 1
fi

if ! command -v sgdisk >/dev/null 2>&1; then
	echo "错误: 需要 gptfdisk（sgdisk）以创建 GPT+ESP"
	echo "  Debian/Ubuntu: sudo apt install gdisk"
	exit 1
fi

if command -v mkfs.fat >/dev/null 2>&1; then
	MKFS="mkfs.fat"
elif command -v mkfs.vfat >/dev/null 2>&1; then
	MKFS="mkfs.vfat"
else
	echo "错误: 需要 dosfstools（mkfs.fat 或 mkfs.vfat）"
	exit 1
fi

if ! command -v mmd >/dev/null 2>&1 || ! command -v mcopy >/dev/null 2>&1; then
	echo "错误: 需要 mtools（mmd、mcopy）"
	exit 1
fi

if ! "$MKFS" --help 2>&1 | grep -q -- '--offset'; then
	echo "错误: $MKFS 需支持 --offset=SECTOR（dosfstools 4.x+）以在 ESP 上建 FAT"
	exit 1
fi

rm -f "$OUT"
dd if=/dev/zero of="$OUT" bs=1M count="$SIZE_MB" status=none

sgdisk -Z -n "1:${PART_START_SECTOR}:0" -t "1:ef00" -c "1:EFI System" "$OUT" >/dev/null

"$MKFS" -F 32 --offset="$PART_START_SECTOR" "$OUT"

MT_IMG="${OUT}@@${ESP_BYTE_OFF}"
mmd -i "$MT_IMG" ::EFI
mmd -i "$MT_IMG" ::EFI/Boot
mcopy -i "$MT_IMG" "$EFI" ::EFI/Boot/bootx64.efi

# OVMF 将 SATA/IDE 盘视为固定硬盘时，常常不会像「可移动介质」那样自动执行
# \EFI\Boot\bootx64.efi，而是进入 UEFI Shell。在 ESP 根目录放置 startup.nsh，
# Shell 会在倒计时结束后自动运行其中的命令，从而进入本系统。
STARTUP_NSHTMP=$(mktemp)
trap 'rm -f "$STARTUP_NSHTMP"' EXIT
# 应用用「当前卷上的程序路径」执行；load 只适用驱动，对 .efi 应用会报 not an image
printf '%s\r\n' \
	'@echo -off' \
	'connect -r' \
	'fs0:' \
	'cd EFI\Boot' \
	'bootx64.efi' \
	> "$STARTUP_NSHTMP"
mcopy -i "$MT_IMG" "$STARTUP_NSHTMP" ::startup.nsh

echo "已生成: $OUT (${SIZE_MB}MB, GPT + FAT32 ESP @ sector ${PART_START_SECTOR})"
ls -lh "$OUT"
sgdisk -p "$OUT" 2>/dev/null | head -18 || true
