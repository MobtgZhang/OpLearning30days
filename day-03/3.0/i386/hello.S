.global start
.code16
start:
    jmp         entry
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
    .fill       18

entry:
    mov     $0,%ax                      /*初始化段寄存器*/
    mov     %ax,%ds
    mov     %ax,%es
    mov     %ax,%ss
    mov     $0x7c00,%sp                 /*装载程序的位置*/
    mov     %dl,(0x7dfe)
/*显示一些消息*/
    mov     $msg,%si
    call    display_message
    mov     $helloos_msg,%si
    call    display_message
/*内存测试*/
    mov     $0x820,%ax
    mov     %ax,%es
    mov     $0x00,%bx

    mov     $0x00,%ch
    mov     $0x00,%dh
    mov     $0x02,%cl
/*现在读取18个扇区的数据*/
readloop:
    mov     $0x00,%si        /*si寄存器记录扇区读取错误的次数*/
    call    read_data

    mov     %es,%ax
    add     $0x0020,%ax
    mov     %ax,%es
    add     $0x01,%cl
    cmp     $18,%cl         /*18个扇区是否读取完毕*/
    jbe     readloop

    mov     $ok_msg,%si
    call    display_message
loop:
    jmp     loop
/*读取一个扇区程序*/
read_data:
    mov     (0x7dfe),%dl
    mov     $0x02,%ah
    mov     $0x01,%al
    int     $0x13
    jnc     over_finish_read_disk
    add     $0x01,%si
    cmp     $0x05,%si
    jae     disk_read_error
    mov     $0x00,%ah
    mov     $0x00,%dl
    int     $0x13
    jmp     read_data
over_finish_read_disk:
    ret
disk_read_error:
    mov     $error_msg,%si
    call    display_message
    jmp     loop
display_message:
    movb    (%si),%al
    add     $1,%si
    cmp     $0,%al
    je      finish
    movb    $0x0e,%ah
    movw    $15,%bx
    int     $0x10
    jmp     display_message
finish:
    ;hlt
    ret
msg:
    .asciz  "\r\n Hello OS BootLoader is running!"
error_msg:
    .asciz  "\r\n Read U flash failed!"  
helloos_msg:
    .asciz  "\r\n Welcome to Hello OS!"
ok_msg:
    .asciz  "\r\n Read U flash disk of 2 sector, ok!"

.org    510
.word   0xaa55
