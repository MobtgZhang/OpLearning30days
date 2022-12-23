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
    .fill       0x12

entry:
    mov     $0x00,%ax                      /*初始化段寄存器*/
    mov     %ax,%ds
    mov     %ax,%es
    mov     %ax,%ss
    mov     $0x7c00,%sp                 /*装载程序的位置*/
    mov     %dl,(0x7dfe)
/*显示一些常见的字符串*/
    mov     $msg,%si
    call    display_message
    mov     $helloos_msg,%si
    call    display_message
/*memory=0x8200,chs=002*/
    mov     $0x800,%ax
    mov     %ax,%es
    mov     $0x00,%bx

    mov     $0x00,%ch
    mov     $0x00,%dh
    mov     $0x01,%cl
/*下面开始读取18个扇区*/
read_loop:
    mov     0x00,%si
    call    read_data
    
    mov     %es,%ax
    mov     $0x0020,%ax
    mov     %ax,%es
    add     $0x01,%cl
    cmp     $0x3F,%cl           /*这里测试是否读取了63个扇区*/
    jbe     read_loop
    mov     $0x01,%cl           /*这里63个扇区读完了，就可以chs中h++*/
    add     $0x01,%dh
    cmp     $0x06,%dh
    jb      read_loop

/*为了测试从U盘读到内存当中读取到内存当中的数据是否有正常，将内存地址0x34fff中的数据
读取出来，在U盘中时候，地址为0x2d000-1的位置
可以使用十六进制编辑这个位置的数据为0xaa
读出内存当中ds:bx=0x34ff:0xf的数据到寄存器cl上与0xaa比较，如果相等，
则说明读取U盘中的数据到内存没有问题，可以进行下一步处理
*/
    push    %ds
    mov     $0x34ff,%ax
    mov     %ax,%ds
    mov     $0xf,%bx
    mov     (%bx),%cl
    pop     %ds
    cmp     $0xaa,%cl
    /*如果相等的话，则跳转到USB数据读取没问题的显示界面*/
    je      usb_msg
    mov     $error_msg,%si
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
    
over_finish_read_disk:
    ret
disk_read_error:
    mov     $error_msg,%si
    call    display_message
    jmp     loop
finish:
    ;hlt
    ret

/*显示字符串的过程函数*/
display_message:
    movb    (%si),%al
    add     $1,%si
    cmp     $0,%al
    je      finish
    movb    $0x0e,%ah
    movw    $15,%bx
    int     $0x10
    jmp     display_message
/*用于显示的一些字符串*/
usb_msg:
    .asciz  "\r\n Last sector of usb read is ok"
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
