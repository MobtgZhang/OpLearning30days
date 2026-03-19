# UEFI x86-64 操作系统（自定义引导，无 GNU-EFI）

基于《30天自制操作系统》的设计思路，使用 **UEFI 启动**、**x86-64**、**自定义 UEFI 接口**（不依赖 GNU-EFI），实现图形与鼠标移动。

## 目录与构建

- **源码**: `src/`（C + 汇编）
- **输出**: `build/Release/` 或 `build/Debug/`
- **默认产物**: `bootx64.efi` + **`uefi-os.img`**（**GPT + FAT32 ESP**，内含 `EFI/Boot/bootx64.efi`，以及根目录 **`startup.nsh`**：在 OVMF+q35+IDE 等「固定硬盘」场景下，固件常进入 Shell 而不自动加载 `bootx64.efi`，由 Shell 执行 `startup.nsh` 再启动应用）
- **构建**:
  - Release（默认）: `make` 或 `make BUILD=Release`
  - Debug（带步骤日志）: `make BUILD=Debug`
  - 仅编译 EFI、不打包镜像: `make efi`
  - 只重新生成镜像（需已有 `.efi`）: `make img`
- **清理**: `make clean`

## 依赖

- `gcc`（x86-64）、`ld`、`objcopy`（需支持 `efi-app-x86_64`）
- **生成镜像**: `gptfdisk`（`sgdisk`）、`dosfstools`（`mkfs.fat`，需支持 `--offset`）、`mtools`（`mmd`、`mcopy`）
- **QEMU 运行**: `qemu-system-x86_64`、OVMF 固件（常见路径：`/usr/share/qemu/OVMF.fd`、`/usr/share/ovmf/`）

镜像大小默认 64MB，可通过环境变量覆盖，例如：`IMG_SIZE_MB=32 make`。

## 运行

### 1. QEMU 加载磁盘镜像（推荐）

```bash
make                    # 生成 build/Release/uefi-os.img
make run                # 或: ./run-qemu.sh Release
# Debug 镜像:
make BUILD=Debug && ./run-qemu.sh Debug
```

`run-qemu.sh` 使用 **q35 + IDE** 挂载 `uefi-os.img`，**`-boot order=d`** 优先从硬盘启动，**`-usb -device usb-tablet`** 提供 **Absolute Pointer**（OVMF 下比默认 PS/2 更易得到稳定指针），**`-serial stdio`** 把客户机 **COM1 (0x3F8)** 接到当前终端。程序在 `efi_main` 开头调用 `serial_init()`，**`LOG_STEP` / `LOG_STEP2` 会始终经串口输出**（Release 也有；Debug 额外写 UEFI ConOut）。**固件若先进入 UEFI Shell**：请等待约 **2 秒**（或按除 ESC 外的键继续），Shell 会执行 ESP 根目录的 `startup.nsh` 再加载 `bootx64.efi`；进入应用后，同一终端里可看到 `[Main]`、`[GOP]`、`[Mouse]` 等串口日志。镜像为 **GPT + ESP**。

**鼠标**：串口若见 `[Mouse] Absolute Pointer OK`，应在 **QEMU 窗口内移动** 即可同步光标；若为 `[Mouse] Simple Pointer OK`，相对位移已与书中 **`my -= mdec.y`** 一致并对 **Y 取反**，且按 `Mode->ResolutionX` 放大量级（×4）。`EFI_SIMPLE_POINTER_PROTOCOL` 须含 **`WaitForInput` + `Mode*`**，否则无法按规范读分辨率。

### 2. 实机 / 其他虚拟机

将 `uefi-os.img` 作为磁盘挂载，或在 U 盘上 `dd` 写入后从 UEFI 启动（注意目标设备勿选错）。

### 3. 环境变量

- `OVMF_CODE`：OVMF 固件路径（未设则自动尝试 `/usr/share/qemu/OVMF.fd`、`/usr/share/ovmf/OVMF.fd` 等）

## 功能

- UEFI 入口（汇编 `boot.S`）→ C 入口 `efi_main`
- GOP：按 **PixelFormat** 用 `gop_pack_rgb` 打包颜色（修正 **BGR/RGB** 与 BitMask），行间距使用 **PixelsPerScanLine**
- 图形布局参考 [30dayMakeOS / 《30天自制操作系统》](https://github.com/yourtion/30dayMakeOS)：`haribote_gfx.c`（`init_screen8` 式桌面与任务栏、`make_window8` 式窗口与标题栏）、`sheet.c`（与书中 **sheet** 相同的 map + z-order 合成）
- 内存：`BootServices->AllocatePool` 分配背景层、窗口层、鼠标图案缓冲
- Simple Pointer：鼠标移动 **sheet_slide** 刷新最上层光标
- **串口调试**：`src/serial.c`（COM1）；**所有构建**下 `LOG_STEP` 均走串口；Debug 另写 ConOut

## 代码结构

| 路径 | 说明 |
|------|------|
| `src/efi/efi.h` | 自定义 UEFI 类型（系统表、引导服务、GOP、Simple Pointer） |
| `src/boot.S` | x86-64 UEFI 入口，调用 `efi_main` |
| `src/main.c` | 初始化 GOP/鼠标，主循环与光标绘制 |
| `src/gop.c/h` | GOP 初始化与像素/清屏 |
| `src/mouse.c/h` | Simple Pointer 初始化与 GetState |
| `src/log.c/h` | Debug 下 ConOut 步骤日志 |
| `scripts/link.ld` | 链接脚本，生成 ELF 再经 objcopy 转为 .efi |
| `scripts/mkimg.sh` | 生成 `uefi-os.img`（FAT32 + EFI 目录） |
