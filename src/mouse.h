#ifndef MOUSE_H
#define MOUSE_H

#include "efi/efi.h"

/**
 * 初始化指针设备：优先 Absolute Pointer（QEMU usb-tablet），否则 Simple Pointer。
 * 与书中最后一天类似：读位移后 mx+=dx, my-=dy（Y 轴反向由内部处理）。
 */
int mouse_init(EFI_SYSTEM_TABLE *ST);

/**
 * 阻塞等待约一帧：对 WaitForInput 做 CheckEvent，未就绪则分片 Stall（适配 OVMF，不依赖定时器事件）。
 * 必须在 mouse_poll 之前调用；无指针设备时等价于 Stall 一帧。
 */
void mouse_wait_frame(EFI_SYSTEM_TABLE *ST);

/**
 * 轮询并更新屏幕坐标（像素，左上角为原点）。
 * ptr_w/ptr_h：光标图层宽高，用于钳制。
 * out_btn 非 NULL 时写入左键：bit0=按下（与 mouse_decode 后 mdec.btn&1 对应）。
 * 返回 1 表示坐标或键状态相对上次成功读数有变化；0 表示无新数据。
 */
int mouse_poll(EFI_SYSTEM_TABLE *ST, INT32 *out_x, INT32 *out_y,
	       UINT32 scr_w, UINT32 scr_h, INT32 ptr_w, INT32 ptr_h,
	       UINT8 *out_btn);

/* WaitForEvent 用（ExitBootServices 前） */
EFI_EVENT mouse_event(void);

#endif
