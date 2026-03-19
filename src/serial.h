/**
 * QEMU / 实机 COM1 (0x3F8) 轮询输出，配合 qemu -serial stdio 在宿主终端看日志
 */
#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
void serial_write(const char *s, unsigned long n);
void serial_put_hex64(unsigned long v);

#endif
