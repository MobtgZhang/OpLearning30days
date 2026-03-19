/**
 * UEFI Graphics Output Protocol：按 PixelFormat 打包颜色，行宽按 PixelsPerScanLine。
 */
#include "gop.h"
#include "log.h"
#include <stddef.h>

typedef EFI_STATUS (__attribute__((ms_abi)) *fn_LocateProtocol)(void *, void *, void **);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_SetMode)(EFI_GRAPHICS_OUTPUT_PROTOCOL *, UINT32);

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
static void *fb_base;
static UINTN fb_size;
static UINTN stride_bytes;
static UINTN pitch_pixels;
static UINT32 width, height;
static UINT32 px_fmt;
static EFI_PIXEL_BITMASK px_mask;

static const EFI_GUID gop_guid = {
	0x9042a9de, 0x23dc, 0x4a38,
	{ 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a }
};

static unsigned popcnt32(UINT32 x)
{
	unsigned n = 0;

	while (x) {
		n++;
		x &= x - 1;
	}
	return n;
}

static unsigned ctz32(UINT32 x)
{
	unsigned n = 0;

	if (!x)
		return 32;
	while ((x & 1u) == 0) {
		n++;
		x >>= 1;
	}
	return n;
}

static UINT32 mask_shift_in(UINT8 v, UINT32 mask)
{
	unsigned lsb, bits;
	UINT32 vv;

	if (!mask)
		return 0;
	lsb = ctz32(mask);
	bits = popcnt32(mask);
	vv = v;
	if (bits < 8)
		vv >>= (8 - bits);
	else if (bits > 8)
		vv <<= (bits - 8);
	return (vv & (((UINT32)1 << bits) - 1)) << lsb;
}

static void setup_stride(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info, EFI_SYSTEM_TABLE *ST)
{
	pitch_pixels = info->PixelsPerScanLine;
	px_fmt = info->PixelFormat;
	px_mask = info->PixelInformation;

	if (px_fmt == PixelRedGreenBlueReserved8BitPerColor ||
	    px_fmt == PixelBlueGreenRedReserved8BitPerColor) {
		stride_bytes = pitch_pixels * 4;
		LOG_STEP(ST, "[GOP] pixel fmt RGB/BGR 32bpp\r\n");
	} else if (px_fmt == PixelBitMask) {
		stride_bytes = pitch_pixels * 4;
		LOG_STEP(ST, "[GOP] pixel fmt BitMask 32bpp (assumed)\r\n");
	} else {
		stride_bytes = pitch_pixels * 4;
		LOG_STEP(ST, "[GOP] pixel fmt other, assume 4 bytes/pixel\r\n");
	}
}

UINT32 gop_pack_rgb(UINT8 r, UINT8 g, UINT8 b)
{
	if (px_fmt == PixelBlueGreenRedReserved8BitPerColor)
		return (UINT32)b | ((UINT32)g << 8) | ((UINT32)r << 16);
	if (px_fmt == PixelRedGreenBlueReserved8BitPerColor)
		return (UINT32)r | ((UINT32)g << 8) | ((UINT32)b << 16);
	if (px_fmt == PixelBitMask)
		return mask_shift_in(r, px_mask.RedMask) |
		       mask_shift_in(g, px_mask.GreenMask) |
		       mask_shift_in(b, px_mask.BlueMask);
	return (UINT32)b | ((UINT32)g << 8) | ((UINT32)r << 16);
}

UINTN gop_pitch_pixels(void)
{
	return pitch_pixels;
}

UINT32 gop_pixel_format(void)
{
	return px_fmt;
}

int gop_init(EFI_SYSTEM_TABLE *ST, UINT32 mode)
{
	EFI_STATUS status;
	void *interface = NULL;

	LOG_STEP(ST, "[GOP] LocateProtocol...\r\n");

	status = ((fn_LocateProtocol)ST->BootServices->LocateProtocol)((void *)&gop_guid, NULL, &interface);
	if (status != EFI_SUCCESS || !interface) {
		LOG_STEP2(ST, "[GOP] LocateProtocol failed, status/interface ", (unsigned long)status, (unsigned long)(UINTN)interface);
		return -1;
	}
	gop = (EFI_GRAPHICS_OUTPUT_PROTOCOL *)interface;

	LOG_STEP(ST, "[GOP] SetMode...\r\n");
	status = ((fn_SetMode)gop->SetMode)(gop, mode);
	if (status != EFI_SUCCESS) {
		LOG_STEP(ST, "[GOP] SetMode failed\r\n");
		return -1;
	}

	fb_base = (void *)(UINTN)gop->Mode->FrameBufferBase;
	fb_size = gop->Mode->FrameBufferSize;
	width   = gop->Mode->Info->HorizontalResolution;
	height  = gop->Mode->Info->VerticalResolution;
	setup_stride(gop->Mode->Info, ST);

	LOG_STEP2(ST, "[GOP] OK res ", (unsigned long)width, (unsigned long)height);
	return 0;
}

void *gop_fb_base(void)  { return fb_base; }
UINTN gop_fb_size(void)  { return fb_size; }
UINT32 gop_width(void)   { return width; }
UINT32 gop_height(void)  { return height; }
UINTN gop_stride(void)   { return stride_bytes; }

void gop_put_pixel(UINT32 x, UINT32 y, UINT32 color)
{
	UINT8 *base;
	UINT32 *row;

	if (!fb_base || x >= width || y >= height)
		return;
	base = (UINT8 *)fb_base;
	row = (UINT32 *)(base + y * stride_bytes);
	row[x] = color;
}

void gop_clear(UINT32 color)
{
	UINT8 *base = (UINT8 *)fb_base;
	UINTN p;

	if (!fb_base)
		return;
	for (UINT32 y = 0; y < height; y++) {
		UINT32 *row = (UINT32 *)(base + y * stride_bytes);

		for (p = 0; p < pitch_pixels; p++)
			row[p] = color;
	}
}

void gop_get_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 *buf)
{
	if (!fb_base || !buf || x + w > width || y + h > height)
		return;
	for (UINT32 dy = 0; dy < h; dy++) {
		UINT32 *row = (UINT32 *)((UINT8 *)fb_base + (y + dy) * stride_bytes);

		for (UINT32 dx = 0; dx < w; dx++)
			*buf++ = row[x + dx];
	}
}

void gop_put_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, const UINT32 *buf)
{
	if (!fb_base || !buf || x + w > width || y + h > height)
		return;
	for (UINT32 dy = 0; dy < h; dy++) {
		UINT32 *row = (UINT32 *)((UINT8 *)fb_base + (y + dy) * stride_bytes);

		for (UINT32 dx = 0; dx < w; dx++)
			row[x + dx] = *buf++;
	}
}
