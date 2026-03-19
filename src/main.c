/**
 * UEFI 主入口：Haribote / 30dayMakeOS 风格「桌面 + 图层 + 窗口」，颜色经 gop_pack_rgb 适配 GOP。
 */
#include "efi/efi.h"
#include "log.h"
#include "serial.h"
#include "gop.h"
#include "mouse.h"
#include "haribote_gfx.h"
#include "sheet.h"
#include <stddef.h>

#define WIN_W        320
#define WIN_H        240
#define MOUSE_COL_INV 0x13579BD0u
#define SHEET_OPAQUE  0xCAFEBABEu
#define EFI_LOADER_DATA 2

typedef EFI_STATUS (__attribute__((ms_abi)) *fn_AllocatePool)(UINTN, UINTN, void **);

static void *pool_alloc(EFI_SYSTEM_TABLE *ST, UINTN bytes)
{
	void *p = NULL;
	fn_AllocatePool ap;

	ap = (fn_AllocatePool)ST->BootServices->AllocatePool;
	if (ap(EFI_LOADER_DATA, bytes, &p) != EFI_SUCCESS)
		return NULL;
	return p;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
	__attribute__((ms_abi));

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	struct SHTCTL ctl;
	struct SHEET *sht_back, *sht_win, *sht_mouse;
	UINT8 *map = NULL;
	UINT32 *buf_back = NULL, *buf_win = NULL, *buf_mouse = NULL;
	UINT32 w, h;
	UINTN pitch;

	(void)ImageHandle;

	serial_init();
	LOG_STEP(SystemTable, "[Main] efi_main start (Haribote-style desktop)\r\n");

	if (gop_init(SystemTable, 0) != 0) {
		LOG_STEP(SystemTable, "[Main] GOP init failed, halt.\r\n");
		for (;;) { __asm__ volatile ("hlt"); }
	}

	if (mouse_init(SystemTable) != 0)
		LOG_STEP(SystemTable, "[Main] no pointer (仅键盘/显示可用)\r\n");

	w = gop_width();
	h = gop_height();
	pitch = gop_pitch_pixels();

	map = (UINT8 *)pool_alloc(SystemTable, (UINTN)w * (UINTN)h);
	buf_back = (UINT32 *)pool_alloc(SystemTable, (UINTN)w * (UINTN)h * sizeof(UINT32));
	buf_win = (UINT32 *)pool_alloc(SystemTable, (UINTN)WIN_W * (UINTN)WIN_H * sizeof(UINT32));
	buf_mouse = (UINT32 *)pool_alloc(SystemTable, 16u * 16u * sizeof(UINT32));
	if (!map || !buf_back || !buf_win || !buf_mouse) {
		LOG_STEP(SystemTable, "[Main] AllocatePool failed\r\n");
		for (;;) { __asm__ volatile ("hlt"); }
	}

	shtctl_init(&ctl, (UINT32 *)(UINTN)gop_fb_base(), pitch, (int)w, (int)h, map);

	sht_back = sheet_alloc(&ctl);
	sht_win = sheet_alloc(&ctl);
	sht_mouse = sheet_alloc(&ctl);
	if (!sht_back || !sht_win || !sht_mouse) {
		LOG_STEP(SystemTable, "[Main] sheet_alloc failed\r\n");
		for (;;) { __asm__ volatile ("hlt"); }
	}

	sheet_setbuf(sht_back, buf_back, (int)w, (int)h, SHEET_OPAQUE);
	sheet_setbuf(sht_win, buf_win, WIN_W, WIN_H, SHEET_OPAQUE);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, MOUSE_COL_INV);

	hrb_init_screen32(buf_back, (int)w, (int)h);
	hrb_make_window32(buf_win, WIN_W, WIN_H, 1);
	/* 客户区：浅灰底（参考书中窗口内部） */
	hrb_boxfill32(buf_win, WIN_W, hrb_palette_color(HRB_COL_FFFFFF), 3,
		      21, WIN_W - 4, WIN_H - 4);

	hrb_init_mouse_cursor32(buf_mouse, MOUSE_COL_INV);

	{
		INT32 mx = (INT32)(w / 2) - 8;
		INT32 my = (INT32)(h / 2) - 8;

		sheet_slide(sht_back, 0, 0);
		sheet_slide(sht_win, (int)(w / 2) - WIN_W / 2, (int)(h / 2) - WIN_H / 2);
		sheet_slide(sht_mouse, mx, my);

		sheet_updown(sht_back, 0);
		sheet_updown(sht_win, 1);
		sheet_updown(sht_mouse, 2);
	}

	/* 保险：整屏 map + 合成一次 */
	sheet_refreshmap(&ctl, 0, 0, (int)w, (int)h, 0);
	sheet_refreshsub(&ctl, 0, 0, (int)w, (int)h, 0, ctl.top);

	LOG_STEP(SystemTable, "[Main] Enter main loop (Haribote day24: wait + poll + sheet_slide)\r\n");

	/* 与 bootpack 拖动一致：mmx/mmy 为上次抓取标题栏时的屏幕坐标 */
	{
		INT32 mmx = -1, mmy = -1;
		UINT8 btn = 0;

		for (;;) {
			INT32 cur_x = sht_mouse->vx0;
			INT32 cur_y = sht_mouse->vy0;

			mouse_wait_frame(SystemTable);
			if (mouse_poll(SystemTable, &cur_x, &cur_y, w, h, 16, 16, &btn))
				sheet_slide(sht_mouse, (int)cur_x, (int)cur_y);

			/* 标题栏 3..20（与 hrb_make_wtitle32 一致），参照 ghosind day24 */
			if (btn & 1u) {
				if (mmx < 0) {
					int wx = (int)cur_x - sht_win->vx0;
					int wy = (int)cur_y - sht_win->vy0;

					if (3 <= wx && wx < sht_win->bxsize && 3 <= wy && wy < 21) {
						mmx = cur_x;
						mmy = cur_y;
					}
				} else {
					sheet_slide(sht_win,
						    sht_win->vx0 + (int)(cur_x - mmx),
						    sht_win->vy0 + (int)(cur_y - mmy));
					mmx = cur_x;
					mmy = cur_y;
				}
			} else {
				mmx = -1;
			}
		}
	}

	return EFI_SUCCESS;
}
