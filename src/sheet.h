/**
 * Haribote 风格图层控制器：map + z-order，合成到 GOP 帧缓冲（UINT32 + pitch）。
 */
#ifndef SHEET_H
#define SHEET_H

#include "efi/efi.h"

#define MAX_SHEETS 32
#define SHEET_USE 1

struct SHTCTL;

struct SHEET {
	UINT32 *buf;
	int bxsize, bysize, vx0, vy0;
	UINT32 col_inv;
	int height;
	int flags;
	struct SHTCTL *ctl;
};

struct SHTCTL {
	UINT32 *vram;
	UINTN pitch_px;
	int xsize, ysize;
	int top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
	UINT8 *map;
};

void shtctl_init(struct SHTCTL *ctl, UINT32 *vram, UINTN pitch_px,
		 int xsize, int ysize, UINT8 *map);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, UINT32 *buf, int xsize, int ysize, UINT32 col_inv);
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);

#endif
