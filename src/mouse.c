/**
 * Simple Pointer / Absolute Pointer（QEMU -device usb-tablet 常见为后者）
 * 《30天》书中鼠标：mx += mdec.x; my -= mdec.y — UEFI RelativeMovementY 与屏幕向下增长相反，故对相对 Y 取反。
 */
#include "mouse.h"
#include "log.h"
#include <stddef.h>

typedef EFI_STATUS (__attribute__((ms_abi)) *fn_LocateProtocol)(void *, void *, void **);

#define MOUSE_KIND_NONE    0
#define MOUSE_KIND_SIMPLE  1
#define MOUSE_KIND_ABS     2

/* 主循环一帧等待：微秒；CheckEvent 分片轮询用 */
#define MOUSE_FRAME_US   16000ULL
#define MOUSE_WAIT_SLICE 500ULL

static EFI_SIMPLE_POINTER_PROTOCOL *simple;
static EFI_ABSOLUTE_POINTER_PROTOCOL *absolu;
static int mouse_kind;
static INT32 last_ax, last_ay;
static char last_abs_valid;
static UINT8 s_last_btn;

static const EFI_GUID simple_pointer_guid = {
	0x31878c87, 0x0b75, 0x11d5,
	{ 0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d }
};

static const EFI_GUID absolute_pointer_guid = {
	0x8D59D32B, 0xC655, 0x4AE9,
	{ 0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43 }
};

typedef EFI_STATUS (__attribute__((ms_abi)) *fn_sp_reset)(EFI_SIMPLE_POINTER_PROTOCOL *, unsigned char);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_sp_get)(EFI_SIMPLE_POINTER_PROTOCOL *, EFI_SIMPLE_POINTER_STATE *);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_ap_reset)(EFI_ABSOLUTE_POINTER_PROTOCOL *, unsigned char);
typedef EFI_STATUS (__attribute__((ms_abi)) *fn_ap_get)(EFI_ABSOLUTE_POINTER_PROTOCOL *, EFI_ABSOLUTE_POINTER_STATE *);

/**
 * OVMF 上 LocateProtocol 有时拿到未 Connect 的实例，USB 平板无数据。
 * 按协议枚举句柄并 ConnectController，再 HandleProtocol（与 EDK2 驱动绑定顺序一致）。
 */
static int try_absolute_via_handles(EFI_SYSTEM_TABLE *ST)
{
	EFI_LOCATE_HANDLE_BUFFER lhb = (EFI_LOCATE_HANDLE_BUFFER)ST->BootServices->LocateHandleBuffer;
	EFI_CONNECT_CONTROLLER conn = (EFI_CONNECT_CONTROLLER)ST->BootServices->ConnectController;
	EFI_HANDLE_PROTOCOL hp = (EFI_HANDLE_PROTOCOL)ST->BootServices->HandleProtocol;
	EFI_FREE_POOL fp = (EFI_FREE_POOL)ST->BootServices->FreePool;
	EFI_HANDLE *handles = NULL;
	UINTN i, n = 0;
	EFI_STATUS status;

	if (!lhb || !conn || !hp || !fp)
		return -1;
	status = lhb(ByProtocol, (void *)&absolute_pointer_guid, NULL, &n, &handles);
	if (status != EFI_SUCCESS || n == 0 || !handles)
		return -1;
	for (i = 0; i < n; i++) {
		void *iface;

		conn(handles[i], NULL, NULL, 1);
		if (hp(handles[i], (void *)&absolute_pointer_guid, &iface) != EFI_SUCCESS || !iface)
			continue;
		absolu = (EFI_ABSOLUTE_POINTER_PROTOCOL *)iface;
		status = ((fn_ap_reset)absolu->Reset)(absolu, 0);
		if (status == EFI_SUCCESS) {
			fp(handles);
			return 0;
		}
		absolu = NULL;
	}
	fp(handles);
	return -1;
}

static int try_simple_via_handles(EFI_SYSTEM_TABLE *ST)
{
	EFI_LOCATE_HANDLE_BUFFER lhb = (EFI_LOCATE_HANDLE_BUFFER)ST->BootServices->LocateHandleBuffer;
	EFI_CONNECT_CONTROLLER conn = (EFI_CONNECT_CONTROLLER)ST->BootServices->ConnectController;
	EFI_HANDLE_PROTOCOL hp = (EFI_HANDLE_PROTOCOL)ST->BootServices->HandleProtocol;
	EFI_FREE_POOL fp = (EFI_FREE_POOL)ST->BootServices->FreePool;
	EFI_HANDLE *handles = NULL;
	UINTN i, n = 0;
	EFI_STATUS status;

	if (!lhb || !conn || !hp || !fp)
		return -1;
	status = lhb(ByProtocol, (void *)&simple_pointer_guid, NULL, &n, &handles);
	if (status != EFI_SUCCESS || n == 0 || !handles)
		return -1;
	for (i = 0; i < n; i++) {
		void *iface;

		conn(handles[i], NULL, NULL, 1);
		if (hp(handles[i], (void *)&simple_pointer_guid, &iface) != EFI_SUCCESS || !iface)
			continue;
		simple = (EFI_SIMPLE_POINTER_PROTOCOL *)iface;
		status = ((fn_sp_reset)simple->Reset)(simple, 0);
		if (status == EFI_SUCCESS) {
			fp(handles);
			return 0;
		}
		simple = NULL;
	}
	fp(handles);
	return -1;
}

/* 书中 PS/2 解析后直接把增量加到 mx/my；QEMU 相对位移往往很小，放大量级 */
static INT32 scale_simple_delta(INT32 cnt)
{
	/* 倍增后再用 Mode->Resolution 微调（有分辨率时略减慢，避免乱跳） */
	long long v = (long long)cnt * 4;

	if (simple && simple->Mode && simple->Mode->ResolutionX > 0 &&
	    simple->Mode->ResolutionX < 8192u)
		v = (long long)cnt * 8000 / (long long)simple->Mode->ResolutionX;
	return (INT32)v;
}

int mouse_init(EFI_SYSTEM_TABLE *ST)
{
	EFI_STATUS status;
	void *interface = NULL;

	mouse_kind = MOUSE_KIND_NONE;
	simple = NULL;
	absolu = NULL;
	last_abs_valid = 0;
	s_last_btn = 0;

	/* ① Absolute：先按句柄绑定（QEMU -device usb-tablet + OVMF） */
	if (try_absolute_via_handles(ST) == 0) {
		mouse_kind = MOUSE_KIND_ABS;
		LOG_STEP(ST, "[Mouse] Absolute Pointer OK (handles+connect)\r\n");
		return 0;
	}

	interface = NULL;
	status = ((fn_LocateProtocol)ST->BootServices->LocateProtocol)((void *)&absolute_pointer_guid, NULL, &interface);
	if (status == EFI_SUCCESS && interface) {
		absolu = (EFI_ABSOLUTE_POINTER_PROTOCOL *)interface;
		status = ((fn_ap_reset)absolu->Reset)(absolu, 0);
		if (status == EFI_SUCCESS) {
			mouse_kind = MOUSE_KIND_ABS;
			LOG_STEP(ST, "[Mouse] Absolute Pointer OK (LocateProtocol)\r\n");
			return 0;
		}
		absolu = NULL;
	}

	/* ② Simple：句柄绑定后回退 LocateProtocol */
	if (try_simple_via_handles(ST) == 0) {
		mouse_kind = MOUSE_KIND_SIMPLE;
		LOG_STEP(ST, "[Mouse] Simple Pointer OK (handles+connect)\r\n");
		return 0;
	}

	interface = NULL;
	LOG_STEP(ST, "[Mouse] Locate Simple Pointer (LocateProtocol)...\r\n");
	status = ((fn_LocateProtocol)ST->BootServices->LocateProtocol)((void *)&simple_pointer_guid, NULL, &interface);
	if (status != EFI_SUCCESS || !interface) {
		LOG_STEP(ST, "[Mouse] no pointer device\r\n");
		return -1;
	}
	simple = (EFI_SIMPLE_POINTER_PROTOCOL *)interface;

	status = ((fn_sp_reset)simple->Reset)(simple, 0);
	if (status != EFI_SUCCESS) {
		LOG_STEP(ST, "[Mouse] Simple Reset failed\r\n");
		simple = NULL;
		return -1;
	}
	mouse_kind = MOUSE_KIND_SIMPLE;
	LOG_STEP(ST, "[Mouse] Simple Pointer OK (LocateProtocol)\r\n");
	return 0;
}

static int poll_absolute(INT32 *ox, INT32 *oy, UINT32 scr_w, UINT32 scr_h, INT32 pw, INT32 ph,
			 UINT8 *out_btn)
{
	EFI_ABSOLUTE_POINTER_STATE st;
	EFI_ABSOLUTE_POINTER_STATE last = {0};
	EFI_STATUS status;
	EFI_ABSOLUTE_POINTER_MODE *mode;
	UINT64 minx, maxx, miny, maxy, cx, cy;
	INT32 px, py;
	int got = 0;

	/* 与 bootpack 一次处理 FIFO 中多件类似：读尽当前排队的状态 */
	for (;;) {
		status = ((fn_ap_get)absolu->GetState)(absolu, &st);
		if (status == EFI_NOT_READY)
			break;
		if (status != EFI_SUCCESS)
			return 0;
		last = st;
		got = 1;
	}
	if (!got)
		return 0;

	mode = absolu->Mode;
	if (!mode)
		return 0;

	minx = mode->AbsoluteMinX;
	maxx = mode->AbsoluteMaxX;
	miny = mode->AbsoluteMinY;
	maxy = mode->AbsoluteMaxY;

	if (maxx <= minx || maxy <= miny)
		return 0;

	cx = last.CurrentX;
	cy = last.CurrentY;
	if (cx < minx)
		cx = minx;
	if (cx > maxx)
		cx = maxx;
	if (cy < miny)
		cy = miny;
	if (cy > maxy)
		cy = maxy;

	px = (INT32)((cx - minx) * (UINT64)(scr_w - (UINT32)pw) / (maxx - minx));
	py = (INT32)((cy - miny) * (UINT64)(scr_h - (UINT32)ph) / (maxy - miny));

	{
		UINT8 btn = (last.ActiveButtons & 1u) ? 1u : 0u;

		if (out_btn)
			*out_btn = btn;
		if (last_abs_valid && px == last_ax && py == last_ay && btn == s_last_btn)
			return 0;
		s_last_btn = btn;
	}

	last_ax = px;
	last_ay = py;
	last_abs_valid = 1;
	*ox = px;
	*oy = py;
	return 1;
}

static int poll_simple(INT32 *ox, INT32 *oy, UINT32 scr_w, UINT32 scr_h, INT32 pw, INT32 ph,
		       UINT8 *out_btn)
{
	EFI_SIMPLE_POINTER_STATE st;
	EFI_STATUS status;
	INT32 x, y;
	long long acc_dx = 0, acc_dy = 0;
	UINT8 btn = s_last_btn;
	int any = 0;

	/* 读尽本帧内积压的相对位移（类比中断里连续 fifo_put 后一次 decode） */
	for (;;) {
		status = ((fn_sp_get)simple->GetState)(simple, &st);
		if (status == EFI_NOT_READY)
			break;
		if (status != EFI_SUCCESS)
			return 0;
		any = 1;
		acc_dx += scale_simple_delta(st.RelativeMovementX);
		acc_dy += scale_simple_delta(st.RelativeMovementY);
		btn = st.LeftButton ? 1u : 0u;
	}
	if (!any)
		return 0;

	x = *ox + (INT32)acc_dx;
	/* 与 bootpack 中 my -= mdec.y 一致 */
	y = *oy - (INT32)acc_dy;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if ((UINT32)x > scr_w - (UINT32)pw)
		x = (INT32)scr_w - pw;
	if ((UINT32)y > scr_h - (UINT32)ph)
		y = (INT32)scr_h - ph;

	if (out_btn)
		*out_btn = btn;
	if (x == *ox && y == *oy && btn == s_last_btn)
		return 0;
	s_last_btn = btn;
	*ox = x;
	*oy = y;
	return 1;
}

int mouse_poll(EFI_SYSTEM_TABLE *ST, INT32 *out_x, INT32 *out_y,
	       UINT32 scr_w, UINT32 scr_h, INT32 ptr_w, INT32 ptr_h,
	       UINT8 *out_btn)
{
	int ret;

	(void)ST;

	if (!out_x || !out_y || mouse_kind == MOUSE_KIND_NONE)
		return 0;

	if (mouse_kind == MOUSE_KIND_ABS)
		ret = poll_absolute(out_x, out_y, scr_w, scr_h, ptr_w, ptr_h, out_btn);
	else
		ret = poll_simple(out_x, out_y, scr_w, scr_h, ptr_w, ptr_h, out_btn);

	return ret;
}

void mouse_wait_frame(EFI_SYSTEM_TABLE *ST)
{
	EFI_CHECK_EVENT chk = (EFI_CHECK_EVENT)ST->BootServices->CheckEvent;
	EFI_STALL stall = (EFI_STALL)ST->BootServices->Stall;
	EFI_EVENT ev;
	UINTN waited = 0;

	if (mouse_kind == MOUSE_KIND_NONE) {
		if (stall)
			stall((UINTN)MOUSE_FRAME_US);
		return;
	}
	ev = (mouse_kind == MOUSE_KIND_ABS) ? absolu->WaitForInput : simple->WaitForInput;
	if (!stall)
		return;
	if (!chk || !ev) {
		stall((UINTN)MOUSE_FRAME_US);
		return;
	}
	/* 有输入则提前返回；否则在约一帧内分片 Stall（不依赖 Timer 事件） */
	while (waited < (UINTN)MOUSE_FRAME_US) {
		if (chk(ev) == EFI_SUCCESS)
			return;
		stall((UINTN)MOUSE_WAIT_SLICE);
		waited += (UINTN)MOUSE_WAIT_SLICE;
	}
}

EFI_EVENT mouse_event(void)
{
	if (mouse_kind == MOUSE_KIND_ABS && absolu)
		return absolu->WaitForInput;
	if (mouse_kind == MOUSE_KIND_SIMPLE && simple)
		return simple->WaitForInput;
	return NULL;
}
