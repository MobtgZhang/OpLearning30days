#include "haribote_gfx.h"
#include "gop.h"

/* 与 30dayMakeOS init_palette 中 0～15 号对应的 RGB */
static const UINT8 pal_rgb[16][3] = {
	{ 0x00, 0x00, 0x00 }, { 0xff, 0x00, 0x00 }, { 0x00, 0xff, 0x00 },
	{ 0xff, 0xff, 0x00 }, { 0x00, 0x00, 0xff }, { 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0xff }, { 0xff, 0xff, 0xff }, { 0xc6, 0xc6, 0xc6 },
	{ 0x84, 0x00, 0x00 }, { 0x00, 0x84, 0x00 }, { 0x84, 0x84, 0x00 },
	{ 0x00, 0x00, 0x84 }, { 0x84, 0x00, 0x84 }, { 0x00, 0x84, 0x84 },
	{ 0x84, 0x84, 0x84 },
};

UINT32 hrb_palette_color(int idx)
{
	if (idx < 0 || idx > 15)
		idx = 0;
	return gop_pack_rgb(pal_rgb[idx][0], pal_rgb[idx][1], pal_rgb[idx][2]);
}

void hrb_boxfill32(UINT32 *buf, int xsize, UINT32 c, int x0, int y0, int x1, int y1)
{
	int x, y;

	for (y = y0; y <= y1; y++)
		for (x = x0; x <= x1; x++)
			buf[y * xsize + x] = c;
}

/* init_screen8：桌面青底 + 任务栏（Haribote 经典布局） */
void hrb_init_screen32(UINT32 *vram, int x, int y)
{
	UINT32 C008484 = hrb_palette_color(HRB_COL_008484);
	UINT32 Cc6c6c6 = hrb_palette_color(HRB_COL_C6C6C6);
	UINT32 Cffffff = hrb_palette_color(HRB_COL_FFFFFF);
	UINT32 C848484 = hrb_palette_color(HRB_COL_848484);
	UINT32 C000000 = hrb_palette_color(HRB_COL_000000);

	hrb_boxfill32(vram, x, C008484, 0, 0, x - 1, y - 29);
	hrb_boxfill32(vram, x, Cc6c6c6, 0, y - 28, x - 1, y - 28);
	hrb_boxfill32(vram, x, Cffffff, 0, y - 27, x - 1, y - 27);
	hrb_boxfill32(vram, x, Cc6c6c6, 0, y - 26, x - 1, y - 1);

	hrb_boxfill32(vram, x, Cffffff, 3, y - 24, 59, y - 24);
	hrb_boxfill32(vram, x, Cffffff, 2, y - 24, 2, y - 4);
	hrb_boxfill32(vram, x, C848484, 3, y - 4, 59, y - 4);
	hrb_boxfill32(vram, x, C848484, 59, y - 23, 59, y - 5);
	hrb_boxfill32(vram, x, C000000, 2, y - 3, 59, y - 3);
	hrb_boxfill32(vram, x, C000000, 60, y - 24, 60, y - 3);

	hrb_boxfill32(vram, x, C848484, x - 47, y - 24, x - 4, y - 24);
	hrb_boxfill32(vram, x, C848484, x - 47, y - 23, x - 47, y - 4);
	hrb_boxfill32(vram, x, Cffffff, x - 47, y - 3, x - 4, y - 3);
	hrb_boxfill32(vram, x, Cffffff, x - 3, y - 24, x - 3, y - 3);
}

/* make_window8：立体边框 + 客户区（与 window.c 一致） */
void hrb_make_window32(UINT32 *buf, int xsize, int ysize, char act)
{
	UINT32 Cc6c6c6 = hrb_palette_color(HRB_COL_C6C6C6);
	UINT32 Cffffff = hrb_palette_color(HRB_COL_FFFFFF);
	UINT32 C848484 = hrb_palette_color(HRB_COL_848484);
	UINT32 C000000 = hrb_palette_color(HRB_COL_000000);

	(void)act;
	hrb_boxfill32(buf, xsize, Cc6c6c6, 0, 0, xsize - 1, 0);
	hrb_boxfill32(buf, xsize, Cffffff, 1, 1, xsize - 2, 1);
	hrb_boxfill32(buf, xsize, Cc6c6c6, 0, 0, 0, ysize - 1);
	hrb_boxfill32(buf, xsize, Cffffff, 1, 1, 1, ysize - 2);
	hrb_boxfill32(buf, xsize, C848484, xsize - 2, 1, xsize - 2, ysize - 2);
	hrb_boxfill32(buf, xsize, C000000, xsize - 1, 0, xsize - 1, ysize - 1);
	hrb_boxfill32(buf, xsize, Cc6c6c6, 2, 2, xsize - 3, ysize - 3);
	hrb_boxfill32(buf, xsize, C848484, 1, ysize - 2, xsize - 2, ysize - 2);
	hrb_boxfill32(buf, xsize, C000000, 0, ysize - 1, xsize - 1, ysize - 1);
	hrb_make_wtitle32(buf, xsize, act);
}

/* 标题栏、关闭按钮；标题文字可后续接字库 */
void hrb_make_wtitle32(UINT32 *buf, int xsize, char act)
{
	static const char closebtn[14][17] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@",
	};
	int x, y;
	UINT32 tc, tbc;
	UINT32 C848484 = hrb_palette_color(HRB_COL_848484);
	UINT32 Cc6c6c6 = hrb_palette_color(HRB_COL_C6C6C6);
	UINT32 Cffffff = hrb_palette_color(HRB_COL_FFFFFF);
	UINT32 C000084 = hrb_palette_color(HRB_COL_000084);

	if (act) {
		tc = Cffffff;
		tbc = C000084;
	} else {
		tc = Cc6c6c6;
		tbc = C848484;
	}
	hrb_boxfill32(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	(void)tc;
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			char ch = closebtn[y][x];
			int ci;

			if (ch == '@')
				ci = HRB_COL_000000;
			else if (ch == '$')
				ci = HRB_COL_848484;
			else if (ch == 'Q')
				ci = HRB_COL_C6C6C6;
			else
				ci = HRB_COL_FFFFFF;
			buf[(5 + y) * xsize + (xsize - 21 + x)] = hrb_palette_color(ci);
		}
	}
}

/* 与 graphic.c init_mouse_cursor8 相同图案，真彩色 + 透明色 */
void hrb_init_mouse_cursor32(UINT32 *mouse, UINT32 col_inv)
{
	static const char cursor[16][17] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***",
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*')
				mouse[y * 16 + x] = hrb_palette_color(HRB_COL_000000);
			else if (cursor[y][x] == 'O')
				mouse[y * 16 + x] = hrb_palette_color(HRB_COL_FFFFFF);
			else
				mouse[y * 16 + x] = col_inv;
		}
	}
}
