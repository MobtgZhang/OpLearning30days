OUTPUT_FORMAT("binary");

SECTIONS {
    .head 0x0 : {
        LONG(64*1024)           /* 0:  stack+.data+heap的大小（4KB的倍数） */
        LONG(0x69726148)        /* 4:  签名 */
        LONG(0)                 /* 8:  mmarea的大小（4KB的倍数） */
        LONG(0x310000)          /* 12: 堆栈初始值 以及 .data转发目标 */
        LONG(SIZEOF(.data))     /* 16: .data的大小 */
        LONG(LOADADDR(.data))   /* 20: .data初始值列的文件位置 */
        LONG(0xE9000000)        /* 24: 0xE9000000 */
        LONG(boot_main - 0x20)  /* 28: 函数入口地址0x20 */
        LONG(0)                 /* 32: heap区域（malloc区域）开始地址 */
    }
    .text : {
        *(.text)
    }

    .data 0x310000 : AT ( ADDR(.text) + SIZEOF(.text)  ) {
        *(.data)
        *(.rodata*)
        *(.bss)
    }

    /DISCARD/ : {
        *(.eh_frame)
    }
}
