/**
 * 8250 / 16550 兼容 UART，COM1 端口 0x3F8（QEMU 默认映射到 -serial stdio）
 */
#include "serial.h"

#define COM1 0x3F8u

static inline void outb(unsigned short port, unsigned char val)
{
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
	unsigned char v;
	__asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
	return v;
}

void serial_init(void)
{
	outb(COM1 + 1, 0x00); /* 关中断 */
	outb(COM1 + 3, 0x80); /* DLAB：设波特率除数 */
	outb(COM1 + 0, 0x01); /* 低字节：115200（QEMU 常用） */
	outb(COM1 + 1, 0x00); /* 高字节 */
	outb(COM1 + 3, 0x03); /* 8N1，关 DLAB */
	outb(COM1 + 2, 0xC7); /* FIFO on，清 FIFO */
	outb(COM1 + 4, 0x0B); /* RTS/DTR 等 */
}

void serial_putc(char c)
{
	/* 等待 THR 空 (LSR bit 5) */
	while ((inb(COM1 + 5) & 0x20u) == 0)
		;
	outb(COM1, (unsigned char)c);
}

void serial_write(const char *s, unsigned long n)
{
	if (!s)
		return;
	for (unsigned long i = 0; i < n; i++)
		serial_putc(s[i]);
}

void serial_puts(const char *s)
{
	if (!s)
		return;
	while (*s)
		serial_putc(*s++);
}

void serial_put_hex64(unsigned long v)
{
	static const char hex[] = "0123456789ABCDEF";

	serial_putc('0');
	serial_putc('x');
	for (int s = 60; s >= 0; s -= 4)
		serial_putc(hex[(v >> s) & 0xf]);
}
