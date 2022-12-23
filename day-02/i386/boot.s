.global start
.code16
start:
    jmp         entry
    /*定义fat12文件格式*/
    .byte       0x00
    .ascii      "helloOSX"      /*启动扇区名称（8字节）*/
    .word       512             /*每个扇区（sector）大小（必须512字节）*/
    .byte	    1				/*簇（cluster）大小（必须为1个扇区）*/
    .word		1				/*FAT起始位置（一般为第一个扇区）*/
    .byte		2				/*FAT个数（必须为2）*/
    .word		224				/*根目录大小（一般为224项）*/
    .word		2880			/*该磁盘大小（必须为2880扇区1440*1024/512）*/
    .byte		0xf0			/*磁盘类型（必须为0xf0）*/
    .word		9				/*FAT的长度（必??9扇区）*/
    .word		18				/*一个磁道（track）有几个扇区（必须为18）*/
    .word		2				/*磁头数（必??2）*/
    .long		0				/*不使用分区，必须是0*/
    .long		2880			/*重写一次磁盘大小*/
    .byte		0,0,0x29		/*意义不明（固定）*/
    .long		0xffffffff		/*（可能是）卷标号码*/
    .ascii		"HARIBOTEOSX"	/*磁盘的名称（必须为11字?，不足填空格）*/
    .ascii		"FAT12   "		/*磁盘格式名称（必??8字?，不足填空格）*/
    .fill   	18				/*先空出18字节*/
entry:
    mov         $0,%ax
    mov         %ax,%ds
    mov         %ax,%es
    mov         %ax,%ss
    mov         $0x7c00,%sp
    mov         $msg_str,%si
    call        puts_str
    mov         $label_str,%si
    call        puts_str
puts_str:
    movb        (%si),%al
    add         $1,%si
    cmp         $0,%al
    je          finish
    movb        $0x0e,%ah
    movw        $15,%bx
    int         $0x10
    jmp         puts_str
finish:
    ;hlt
    ret
msg_str:
    .asciz      "\r\n BootLoader is running!"

label_str:
    .asciz      "\r\n This is the Hello OS demo!"

    .org        510
    .word       0xaa55

