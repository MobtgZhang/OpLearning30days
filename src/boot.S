/**
 * UEFI x86-64 入口
 * 固件调用约定: RCX = ImageHandle, RDX = SystemTable (MS ABI)
 */
	.text
	.globl	_start
	.type	_start, @function
	.code64

_start:
	/* 固件已按 MS x64 ABI 传入 RCX=ImageHandle, RDX=SystemTable；
	 * C 代码用 -mabi=ms 编译，efi_main 参数在 RCX, RDX */
	call	efi_main

	/* 若返回则挂起 */
1:	hlt
	jmp	1b

	.size	_start, . - _start

	.section .note.GNU-stack,"",%progbits
