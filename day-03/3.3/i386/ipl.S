.text
.code16
jmp entry
.byte   0x90
.ascii "HELLOIPL"
.word   512 # sector size(should be 512 byte)
.byte   1 # clustor size (should be 1 sector)
.word   1 # start sector of FAT(normally 1 sector)
.byte   2 # number of FAT (should be 2)
.word   224 # size of root directory (normally 224)
.word   2880 # size of drive(should be2880 sector)
.byte   0xf0 # media type(should be 0xf0)
.word   9 # length of FAT area(should be 9 sector)
.word   18 # number of sector per track (should be 18 sector)
.word   2 # number of head(should be 2)
.int    0 # partion(should be 0 if not use partion)
.int    2880 # size of dirve
.byte   0,0,0x29 # unknown
.int    0xffffffff # maybe serial number ofvolume
.ascii  "HELLO-OS   " # disk name(11byte)
.ascii  "FAT12   " # name of format(8byte)
.skip   18,0 # padding?

.set CYLS, 10
.set OTHER_CYLS,0xff0

entry:
    # 初始化寄存器
    movw    $0,%ax
    movw    %ax,%es
    movw    %ax,%ss
    # 将栈顶寄存器装载到内存0x7c00地址处
    movw    $0x7c00,%sp
    movw    %ax,%ds    
    # 装载磁盘
    movw    $0x0820,%ax
    movw    %ax,%es
    movb    $0,%ch # 0柱面
    movb    $0,%dh # 0磁道
    movb    $2,%cl # 2扇区

read_loop:
    movw    $0,%si

retry:
    movb    $0x02,%ah
    movb    $1,%al
    movw    $0,%bx
    movb    $0x00,%dl
    int     $0x13
    jnc     next
    
    add     $1,%si
    cmp     $5,%si
    jae     error
    movb    $0,%ah
    movb    $0,%dl
    int     $0x13
    jmp     retry

next:
    movw    %es,%ax
    add     $0x20,%ax
    movw    %ax,%es
    add     $0x01,%cl
    # 比较是否读取到了第18个扇区
    cmp     $0x12,%cl
    jbe     read_loop
    movb    $0x01,%cl
    add     $0x01,%dh
    cmp     $0x02,%dh
    jb      read_loop

    # 重新设置
    movb    $0x00,%dh
    add     $0x01,%ch
    cmp     $CYLS,%ch
    jb      read_loop
    movb    $CYLS,(OTHER_CYLS)
    jmp     0xc200

finish:
    hlt
    jmp     finish
error:
    movw    $error_msg,%si
    call display_msg_func

display_msg_func:
    movb    (%si),%al
    addw    $0x01,%si
    cmpb    $0x00,%al
    je      finish
    movb    $0x0e,%ah
    movw    $0x000e,%bx
    int     $0x10
    jmp     display_msg_func
error_msg:
    .string     "\r\n Load error.\r\n"

.org    0x01fe
.byte   0x55,0xaa
