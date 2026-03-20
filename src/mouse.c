/**
 * UEFI 鼠标输入 — 多层回退策略:
 *
 *  层1: EFI_USB_IO_PROTOCOL 直接同步中断读取 HID 报告
 *       (最可靠, 不依赖 OVMF 是否包含 UsbMouseDxe)
 *
 *  层2: EFI_SIMPLE_POINTER_PROTOCOL / EFI_ABSOLUTE_POINTER_PROTOCOL
 *       (如果 OVMF 包含 UsbMouseDxe / Ps2MouseDxe)
 *
 * QEMU usb-tablet 的 HID 报告格式 (6 字节, absolute):
 *   [0] buttons  [1:2] X(LE16, 0-32767)  [3:4] Y(LE16, 0-32767)  [5] wheel
 * QEMU usb-mouse 的 HID 报告格式 (3 字节, relative):
 *   [0] buttons  [1] dx(INT8)  [2] dy(INT8)
 *
 * 参考: 30dayMakeOS/30_day bootpack.c (mx += mdec.x, sheet_slide)
 */
#include "mouse.h"
#include "log.h"
#include "serial.h"
#include <stddef.h>

typedef EFI_STATUS (__attribute__((ms_abi)) *fn_LocateProtocol)(void *, void *, void **);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_sp_reset)(EFI_SIMPLE_POINTER_PROTOCOL *, unsigned char);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_sp_get)(EFI_SIMPLE_POINTER_PROTOCOL *, EFI_SIMPLE_POINTER_STATE *);

#define MOUSE_KIND_NONE     0
#define MOUSE_KIND_SIMPLE   1
#define MOUSE_KIND_ABS      2
#define MOUSE_KIND_USB_RAW  3

#define MOUSE_FRAME_US  16000ULL

static const EFI_GUID simple_pointer_guid = {
	0x31878c87, 0x0b75, 0x11d5,
	{ 0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d }
};
static const EFI_GUID usb_io_guid = {
	0x2B2F68D6, 0x0CD2, 0x44CF,
	{ 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 }
};

static int s_kind;
static EFI_SIMPLE_POINTER_PROTOCOL   *s_sp;
static EFI_ABSOLUTE_POINTER_PROTOCOL *s_ap;
static EFI_USB_IO_PROTOCOL           *s_usbio;

static UINT8 s_ep_addr;
static UINTN s_ep_pkt_size;
static int s_usb_is_abs;

static UINT8 s_last_btn;
static EFI_EVENT s_frame_timer;

/* ---------- USB IO 直接 HID 读取 ---------- */

static int find_usb_hid_mouse(EFI_SYSTEM_TABLE *ST)
{
	EFI_BOOT_SERVICES *BS = ST->BootServices;
	EFI_LOCATE_HANDLE_BUFFER lhb = (EFI_LOCATE_HANDLE_BUFFER)BS->LocateHandleBuffer;
	EFI_HANDLE_PROTOCOL hp = (EFI_HANDLE_PROTOCOL)BS->HandleProtocol;
	EFI_CONNECT_CONTROLLER conn = (EFI_CONNECT_CONTROLLER)BS->ConnectController;
	EFI_FREE_POOL fp = (EFI_FREE_POOL)BS->FreePool;
	EFI_HANDLE *handles = NULL;
	UINTN n = 0, i;
	EFI_STATUS status;

	if (!lhb || !hp || !fp) return -1;

	/* ConnectAll: 确保所有 USB 设备枚举完毕 */
	{
		EFI_HANDLE *all = NULL;
		UINTN an = 0;
		if (lhb(AllHandles, NULL, NULL, &an, &all) == EFI_SUCCESS && all) {
			for (i = 0; i < an && conn; i++)
				conn(all[i], NULL, NULL, 1);
			fp(all);
		}
	}

	status = lhb(ByProtocol, (void *)&usb_io_guid, NULL, &n, &handles);
	if (status != EFI_SUCCESS || !handles || n == 0)
		return -1;

	for (i = 0; i < n; i++) {
		void *iface = NULL;
		EFI_USB_IO_PROTOCOL *uio;
		EFI_USB_INTERFACE_DESCRIPTOR idesc;
		EFI_USB_ENDPOINT_DESCRIPTOR edesc;
		UINT8 j;

		if (hp(handles[i], (void *)&usb_io_guid, &iface) != EFI_SUCCESS || !iface)
			continue;
		uio = (EFI_USB_IO_PROTOCOL *)iface;

		if (uio->UsbGetInterfaceDescriptor(uio, &idesc) != EFI_SUCCESS)
			continue;

		if (idesc.InterfaceClass != 3)
			continue;

		for (j = 0; j < idesc.NumEndpoints; j++) {
			if (uio->UsbGetEndpointDescriptor(uio, j, &edesc) != EFI_SUCCESS)
				continue;

			if ((edesc.EndpointAddress & 0x80) && (edesc.Attributes & 0x03) == 0x03) {
				s_usbio = uio;
				s_ep_addr = edesc.EndpointAddress;
				s_ep_pkt_size = edesc.MaxPacketSize;
				s_usb_is_abs = (idesc.InterfaceProtocol != 2 && s_ep_pkt_size >= 6) ? 1 : 0;

				serial_puts("[Mouse] USB HID ep=0x");
				serial_put_hex64(s_ep_addr);
				serial_puts(s_usb_is_abs ? " absolute\r\n" : " relative\r\n");

				fp(handles);
				s_kind = MOUSE_KIND_USB_RAW;
				return 0;
			}
		}
	}

	fp(handles);
	return -1;
}

/* ---------- USB HID 轮询 ---------- */

static int poll_usb_raw(INT32 *ox, INT32 *oy, UINT32 scr_w, UINT32 scr_h,
			INT32 pw, INT32 ph, UINT8 *out_btn)
{
	UINT8 buf[64];
	UINTN len = s_ep_pkt_size;
	UINT32 usb_status = 0;
	EFI_STATUS st;
	INT32 x, y;
	UINT8 btn;

	if (!s_usbio || len == 0 || len > 64)
		return 0;

	st = s_usbio->UsbSyncInterruptTransfer(s_usbio, s_ep_addr, buf, &len, 16, &usb_status);
	if (st != EFI_SUCCESS || len == 0)
		return 0;

	btn = buf[0] & 0x07;

	if (s_usb_is_abs && len >= 5) {
		UINT16 raw_x = (UINT16)buf[1] | ((UINT16)buf[2] << 8);
		UINT16 raw_y = (UINT16)buf[3] | ((UINT16)buf[4] << 8);

		x = (INT32)((UINT64)raw_x * (UINT64)(scr_w - (UINT32)pw) / 32767ULL);
		y = (INT32)((UINT64)raw_y * (UINT64)(scr_h - (UINT32)ph) / 32767ULL);
	} else if (len >= 3) {
		INT32 dx = (INT32)(signed char)buf[1];
		INT32 dy = (INT32)(signed char)buf[2];

		x = *ox + dx * 2;
		y = *oy + dy * 2;
	} else {
		return 0;
	}

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if ((UINT32)x > scr_w - (UINT32)pw) x = (INT32)scr_w - pw;
	if ((UINT32)y > scr_h - (UINT32)ph) y = (INT32)scr_h - ph;

	if (out_btn)
		*out_btn = btn & 1u;

	if (x == *ox && y == *oy && (btn & 1u) == s_last_btn)
		return 0;

	s_last_btn = btn & 1u;
	*ox = x;
	*oy = y;
	return 1;
}

/* ---------- Simple Pointer 回退轮询 ---------- */

static INT32 scale_sp(INT32 cnt)
{
	long long v = (long long)cnt * 4;
	if (s_sp && s_sp->Mode && s_sp->Mode->ResolutionX > 0 && s_sp->Mode->ResolutionX < 8192u)
		v = (long long)cnt * 8000 / (long long)s_sp->Mode->ResolutionX;
	return (INT32)v;
}

static int poll_simple(INT32 *ox, INT32 *oy, UINT32 scr_w, UINT32 scr_h,
		       INT32 pw, INT32 ph, UINT8 *out_btn)
{
	EFI_SIMPLE_POINTER_STATE st;
	EFI_STATUS status;
	INT32 x, y;
	long long acc_dx = 0, acc_dy = 0;
	UINT8 btn = s_last_btn;
	int any = 0;

	for (;;) {
		status = ((fn_sp_get)s_sp->GetState)(s_sp, &st);
		if (status == EFI_NOT_READY) break;
		if (status != EFI_SUCCESS) return 0;
		any = 1;
		acc_dx += scale_sp(st.RelativeMovementX);
		acc_dy += scale_sp(st.RelativeMovementY);
		btn = st.LeftButton ? 1u : 0u;
	}
	if (!any) return 0;

	x = *ox + (INT32)acc_dx;
	y = *oy - (INT32)acc_dy;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if ((UINT32)x > scr_w - (UINT32)pw) x = (INT32)scr_w - pw;
	if ((UINT32)y > scr_h - (UINT32)ph) y = (INT32)scr_h - ph;
	if (out_btn) *out_btn = btn;
	if (x == *ox && y == *oy && btn == s_last_btn) return 0;
	s_last_btn = btn;
	*ox = x;
	*oy = y;
	return 1;
}

/* ---------- 公共接口 ---------- */

int mouse_init(EFI_SYSTEM_TABLE *ST)
{
	EFI_STATUS status;
	EFI_CREATE_EVENT create_fn;

	s_kind = MOUSE_KIND_NONE;
	s_sp = NULL;
	s_ap = NULL;
	s_usbio = NULL;
	s_last_btn = 0;
	s_frame_timer = NULL;

	/* 层1: USB IO 直接 HID — 最可靠, 不依赖 OVMF 的鼠标驱动 */
	if (find_usb_hid_mouse(ST) == 0) {
		LOG_STEP(ST, "[Mouse] USB HID raw mode OK\r\n");
	}

	/* 层2: Simple Pointer (仅在 USB IO 不可用时作为回退) */
	if (s_kind == MOUSE_KIND_NONE) {
		void *iface = NULL;
		fn_LocateProtocol lp = (fn_LocateProtocol)ST->BootServices->LocateProtocol;
		if (lp && lp((void *)&simple_pointer_guid, NULL, &iface) == EFI_SUCCESS && iface) {
			s_sp = (EFI_SIMPLE_POINTER_PROTOCOL *)iface;
			((fn_sp_reset)s_sp->Reset)(s_sp, 0);
			s_kind = MOUSE_KIND_SIMPLE;
			LOG_STEP(ST, "[Mouse] Simple Pointer fallback\r\n");
		}
	}

	if (s_kind == MOUSE_KIND_NONE) {
		LOG_STEP(ST, "[Mouse] no pointer device\r\n");
		return -1;
	}

	create_fn = (EFI_CREATE_EVENT)ST->BootServices->CreateEvent;
	if (create_fn) {
		status = create_fn(EVT_TIMER, 0, NULL, NULL, &s_frame_timer);
		if (status != EFI_SUCCESS)
			s_frame_timer = NULL;
	}

	return 0;
}

int mouse_poll(EFI_SYSTEM_TABLE *ST, INT32 *out_x, INT32 *out_y,
	       UINT32 scr_w, UINT32 scr_h, INT32 ptr_w, INT32 ptr_h,
	       UINT8 *out_btn)
{
	(void)ST;
	if (!out_x || !out_y || s_kind == MOUSE_KIND_NONE)
		return 0;

	if (s_kind == MOUSE_KIND_USB_RAW)
		return poll_usb_raw(out_x, out_y, scr_w, scr_h, ptr_w, ptr_h, out_btn);
	if (s_kind == MOUSE_KIND_SIMPLE && s_sp)
		return poll_simple(out_x, out_y, scr_w, scr_h, ptr_w, ptr_h, out_btn);
	return 0;
}

void mouse_wait_frame(EFI_SYSTEM_TABLE *ST)
{
	EFI_BOOT_SERVICES *BS = ST->BootServices;
	EFI_STALL stall_fn = (EFI_STALL)BS->Stall;

	if (s_kind == MOUSE_KIND_USB_RAW) {
		/* UsbSyncInterruptTransfer 自带 16ms 超时, 已充当帧等待 */
		return;
	}

	/* 标准协议: WaitForEvent + timer */
	{
		EFI_WAIT_FOR_EVENT wait_fn = (EFI_WAIT_FOR_EVENT)BS->WaitForEvent;
		EFI_SET_TIMER set_timer_fn = (EFI_SET_TIMER)BS->SetTimer;
		EFI_EVENT mouse_ev = NULL;
		EFI_EVENT events[2];
		UINTN index = 0;

		if (s_kind == MOUSE_KIND_SIMPLE && s_sp)
			mouse_ev = s_sp->WaitForInput;
		else if (s_kind == MOUSE_KIND_ABS && s_ap)
			mouse_ev = s_ap->WaitForInput;

		if (!wait_fn || !mouse_ev || !set_timer_fn || !s_frame_timer) {
			if (stall_fn) stall_fn((UINTN)MOUSE_FRAME_US);
			return;
		}
		set_timer_fn(s_frame_timer, TimerCancel, 0);
		set_timer_fn(s_frame_timer, TimerRelative, (UINT64)MOUSE_FRAME_US * 10ULL);
		events[0] = mouse_ev;
		events[1] = s_frame_timer;
		wait_fn(2, events, &index);
		set_timer_fn(s_frame_timer, TimerCancel, 0);
	}
}

EFI_EVENT mouse_event(void)
{
	if (s_kind == MOUSE_KIND_SIMPLE && s_sp)
		return s_sp->WaitForInput;
	if (s_kind == MOUSE_KIND_ABS && s_ap)
		return s_ap->WaitForInput;
	return NULL;
}
