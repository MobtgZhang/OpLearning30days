/* UEFI x86-64 可执行文件链接脚本（输出 ELF，再由 objcopy 转为 .efi）*/
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)

/* PE ImageBase 为 0 且重定位被剥离时，多数固件 LoadImage 会返回 Unsupported。
 * 使用非零虚地址与 gnu-efi 等工程常见基址一致。 */
SECTIONS
{
	/* ImageBase 与首段之间需留一页：PE 首段 RVA 不能为 0（否则与 MZ/PE 头重叠） */
	.text 0x201000 : ALIGN(4096) {
		*(.text .text.*)
	}
	.data : ALIGN(4096) {
		*(.data .data.*)
		*(.rodata .rodata.*)
	}
	.bss : ALIGN(4096) {
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		__bss_end = .;
		. = ALIGN(4096);
		__heap_start = .;
	}
	/DISCARD/ : {
		*(.note*)
		*(.comment)
	}
	_end = .;
}
