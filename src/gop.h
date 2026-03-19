#ifndef GOP_H
#define GOP_H

#include "efi/efi.h"

/* 初始化 GOP，设置指定模式（0=默认），返回 0 成功 */
int gop_init(EFI_SYSTEM_TABLE *ST, UINT32 mode);

/* 获取帧缓冲基址、大小、宽度、高度、每行字节数 */
void *gop_fb_base(void);
UINTN gop_fb_size(void);
UINT32 gop_width(void);
UINT32 gop_height(void);
UINTN gop_stride(void);
UINTN gop_pitch_pixels(void);
UINT32 gop_pixel_format(void);
/* sRGB 0～255 → 帧缓冲原生 UINT32（修正 BGR/RGB，避免整屏发黑） */
UINT32 gop_pack_rgb(UINT8 r, UINT8 g, UINT8 b);

/* 画一个像素（32bpp 行布局；与 GOP 常见模式一致） */
void gop_put_pixel(UINT32 x, UINT32 y, UINT32 color);

/* 清屏为指定颜色 */
void gop_clear(UINT32 color);

/* 读/写矩形区域（用于保存/恢复光标下背景，避免整屏重绘） */
void gop_get_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 *buf);
void gop_put_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, const UINT32 *buf);

#endif
