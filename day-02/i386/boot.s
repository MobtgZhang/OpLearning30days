.code16
.global start
start:
    jmp entry
/*这里定义了基本的fat12软盘的结构*/
        /*定义fat12文件格式*/
    .byte       0x00
    .ascii      "HelloOSX"
    .word       0x200
    .byte       0x01
    .word       0x01
    .byte       0x02
    .word       0xe0
    .word       0xb40
    .byte       0xf0
    .word       0x09
    .word       0x12
    .word       0x02
    .long       0x00
    .long       0xb40
    .byte       0x00,0x00,0x29
    .long       0xffffffff
    .ascii      "HelloOSDisk"
    .ascii      "fat12  "
    .fill       0x12
entry:
/*初始化寄存器*/
    movw    $0,%ax
    movw    %ax,%ds
    movw    %ax,%es
    movw    %ax,%ss
    movw    $0x7c00,%sp
/*显示字符串*/
    movw    $hello_msg,%si
    call    display_msg
finish:
    hlt
    jmp     finish
display_msg:
    movb    (%si),%al
    addw    $1,%si
    cmpb    $0,%al
    je      over_ret
    movb    $0x0e,%ah
    movw    $15,%bx
    int     $0x10
    jmp     display_msg
over_ret:
    hlt
    ret
hello_msg:
    .string     "\r\n This is Hello OS."

.org        0x01fe
.byte       0x55,0xaa
