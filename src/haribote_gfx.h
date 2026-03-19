/**
 * 《30天自制操作系统》风格图形：调色板索引与窗口绘制（真彩色 UINT32 缓冲区）
 * 参考 yourtion/30dayMakeOS graphic.c + window.c 的逻辑，颜色通过 gop_pack_rgb 转换。
 */
#ifndef HARIBOTE_GFX_H
#define HARIBOTE_GFX_H

#include "efi/efi.h"

enum {
	HRB_COL_000000 = 0,
	HRB_COL_FF0000,
	HRB_COL_00FF00,
	HRB_COL_FFFF00,
	HRB_COL_0000FF,
	HRB_COL_FF00FF,
	HRB_COL_00FFFF,
	HRB_COL_FFFFFF,
	HRB_COL_C6C6C6,
	HRB_COL_840000,
	HRB_COL_008400,
	HRB_COL_848400,
	HRB_COL_000084,
	HRB_COL_840084,
	HRB_COL_008484,
	HRB_COL_848484,
};

UINT32 hrb_palette_color(int idx);

void hrb_boxfill32(UINT32 *buf, int xsize, UINT32 c, int x0, int y0, int x1, int y1);
void hrb_init_screen32(UINT32 *vram, int x, int y);
void hrb_make_window32(UINT32 *buf, int xsize, int ysize, char act);
void hrb_make_wtitle32(UINT32 *buf, int xsize, char act);
void hrb_init_mouse_cursor32(UINT32 *mouse, UINT32 col_inv);

#endif
