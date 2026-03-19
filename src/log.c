/**
 * COM1 串口日志（主） + Debug 下 ConOut（辅）
 */
#include "efi/efi.h"
#include "log.h"
#include "serial.h"
#include <stddef.h>

static UINT16 conout_buf[256];

static void conout_utf16(EFI_SYSTEM_TABLE *ST, const UINT16 *wstr)
{
	if (!ST || !ST->ConOut || !wstr)
		return;
	typedef EFI_STATUS (__attribute__((ms_abi)) *fn_OutputString)(void *, UINT16 *);
	((fn_OutputString)ST->ConOut->OutputString)(ST->ConOut, (UINT16 *)wstr);
}

void log_conout_line(EFI_SYSTEM_TABLE *ST, const char *msg)
{
	size_t i;

	if (!msg)
		return;
	serial_puts(msg);
	if (!ST || !ST->ConOut)
		return;
	i = 0;
	while (msg[i] && i < 255) {
		conout_buf[i] = (UINT16)(unsigned char)msg[i];
		i++;
	}
	conout_buf[i] = 0;
	conout_utf16(ST, conout_buf);
}

void log_step(EFI_SYSTEM_TABLE *ST, const char *msg)
{
	if (!msg)
		return;
	serial_puts(msg);
#ifdef DEBUG_BUILD
	if (!ST || !ST->ConOut)
		return;
	size_t i = 0;
	while (msg[i] && i < 255) {
		conout_buf[i] = (UINT16)(unsigned char)msg[i];
		i++;
	}
	conout_buf[i] = 0;
	conout_utf16(ST, conout_buf);
#else
	(void)ST;
#endif
}

void log_step2(EFI_SYSTEM_TABLE *ST, const char *msg, unsigned long a, unsigned long b)
{
	(void)ST;
	if (!msg)
		return;
	serial_puts(msg);
	serial_put_hex64(a);
	serial_putc(' ');
	serial_put_hex64(b);
	serial_puts("\r\n");
#ifdef DEBUG_BUILD
	if (!ST || !ST->ConOut || !msg)
		return;
	size_t i = 0;
	while (msg[i] && i < 200) {
		conout_buf[i] = (UINT16)(unsigned char)msg[i];
		i++;
	}
	const char hex[] = "0123456789ABCDEF";
	unsigned long v = a;
	conout_buf[i++] = ' ';
	conout_buf[i++] = '0';
	conout_buf[i++] = 'x';
	for (int s = 60; s >= 0; s -= 4)
		conout_buf[i++] = hex[(v >> s) & 0xf];
	conout_buf[i++] = ' ';
	v = b;
	conout_buf[i++] = '0';
	conout_buf[i++] = 'x';
	for (int s = 60; s >= 0; s -= 4)
		conout_buf[i++] = hex[(v >> s) & 0xf];
	conout_buf[i++] = '\r';
	conout_buf[i++] = '\n';
	conout_buf[i] = 0;
	conout_utf16(ST, conout_buf);
#else
	(void)ST;
#endif
}
