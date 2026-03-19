/**
 * 日志：默认经 COM1 输出到 QEMU -serial stdio（Release/Debug 均有）
 * Debug 构建额外写 UEFI ConOut（图形/文本控制台）
 */
#ifndef LOG_H
#define LOG_H

#include "efi/efi.h"

#define LOG_STEP(ST, msg)          log_step((ST), (msg))
#define LOG_STEP2(ST, msg, a, b)   log_step2((ST), (msg), (a), (b))

void log_step(EFI_SYSTEM_TABLE *ST, const char *msg);
void log_step2(EFI_SYSTEM_TABLE *ST, const char *msg, unsigned long a, unsigned long b);

/** 串口 + UEFI 文本控制台（与 DEBUG_BUILD 无关，便于 Release 下调试） */
void log_conout_line(EFI_SYSTEM_TABLE *ST, const char *msg);

#endif
