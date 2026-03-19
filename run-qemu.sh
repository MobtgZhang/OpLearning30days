#!/bin/sh
# 使用 QEMU + OVMF 加载 uefi-os.img 运行
# 依赖: qemu-system-x86_64, OVMF 固件

BUILD="${1:-Release}"
IMG="build/${BUILD}/uefi-os.img"

# 自动查找 OVMF：优先用环境变量，再按常见路径尝试
if [ -n "$OVMF_CODE" ] && [ -f "$OVMF_CODE" ]; then
	:
elif [ -f /usr/share/qemu/OVMF.fd ]; then
	OVMF_CODE=/usr/share/qemu/OVMF.fd
elif [ -f /usr/share/ovmf/OVMF.fd ]; then
	OVMF_CODE=/usr/share/ovmf/OVMF.fd
elif [ -f /usr/share/ovmf/OVMF_CODE.fd ]; then
	OVMF_CODE=/usr/share/ovmf/OVMF_CODE.fd
elif [ -f /usr/share/edk2/ovmf/OVMF_CODE.fd ]; then
	OVMF_CODE=/usr/share/edk2/ovmf/OVMF_CODE.fd
else
	OVMF_CODE=""
fi

if [ ! -f "$IMG" ]; then
	echo "未找到 $IMG，请先执行: make BUILD=$BUILD 或 make img BUILD=$BUILD"
	exit 1
fi

if [ -z "$OVMF_CODE" ] || [ ! -f "$OVMF_CODE" ]; then
	echo "未找到 OVMF 固件。请安装 ovmf 或 edk2-ovmf，或设置 OVMF_CODE 指向 .fd 文件。"
	echo "常见路径: /usr/share/qemu/OVMF.fd 或 /usr/share/ovmf/"
	exit 1
fi

echo "QEMU: 磁盘镜像 $IMG"
echo "串口日志: COM1 -> 本终端（program LOG_STEP）"
echo "鼠标: -usb -device usb-tablet → UEFI Absolute Pointer（与书中最后一天「可拖窗口」体验接近）"
# q35 + IDE：OVMF 对固定硬盘需 GPT+ESP（见 scripts/mkimg.sh）
exec qemu-system-x86_64 -machine q35 -bios "$OVMF_CODE" \
	-usb -device usb-tablet \
	-serial stdio \
	-boot order=d \
	-drive file="$IMG",format=raw,if=ide,media=disk,index=0 \
	-net none -m 128
