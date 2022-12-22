/*设定启动区*/
#define CYLS        0x0ff0              
#define LEDS        0x0ff1
/*关于颜色数目的信息，颜色的位数*/
#define VMODE       0x0ff2
/*分辨率的X*/
#define SCRANX      0x0ff4
/*分辨率的Y*/
#define SCRANY      0x0ff6
/*图像开始缓冲区的开始地址*/
#define VRAM        0x0ff8

#define BOTPAK      0x00280000
#define DSKCAC      0x00100000
#define DSKCAC0     0x00008000

.global start
start:
    .code16
    jmp     entry
kernel_msg:
    .asciz  "\r\n Hello OS kernel is running ! \r\n"
another_display_msg:
    .asciz  "\r\n Another video mode, the screen don't support!"
/*显示函数*/
display_msg_func:
    movb    (%si),%al
    add     $1,%si
    cmp     $0,%al
    je      over_ret
    movb    $0x0e,%ah
    movw    $15,%bx
    int     $0x10
    jmp     display_msg_func
/*表示函数返回*/
over_ret:
    ret
entry:
    mov     $0,%ax
    mov     %ax,%ds
    mov     %ax,%es
    mov     %ax,%ss

    movw    $kernel_msg,%si
    call    display_msg_func
/*设置显示器模式为320x200x8bit彩色模式*/
    mov     $0x13,%al
    mov     $0x00,%ah
    int     $0x10

/*显示字符串*/
    movw    $another_display_msg,%si
    call    display_msg_func
/*获取键盘状态信息，并保存*/
    mov     $0x02,%ah
    int     $0x16
    mov     %al,(LEDS)
/*在地址0x0ff0处保存颜色信息*/
    movb    $10,(CYLS)
    movb    $8,(VMODE)
    movw    $320,(SCRANX)
    movw    $200,(SCRANY)
    movl    $0x000a0000,(VRAM)
/*将0xff写入到端口0x21,端口0xa1*/
    movb    $0xff,%al
    outb    %al,$0x21
    nop
    out     %al,$0xa1
    cli                                 /*屏蔽当前中断*/
/*使得A20可用，步骤1*/
    seta20.1:
    inb     $0x64,%al                   /*从端口0x64读入数据，并将其保存到al当中*/
    testb   $0x02,%al                   /*测试字节0b0000 0010,是否读取完毕*/
    jnz     seta20.1

    movb    $0xd1,%al
    outb    %al,$0x60
/*将cr0的bit0打开之后，则实现了32位模式*/
    lgdt    gdtdesc
    movl    %cr0,%eax
    andl    $0x7fffffff,%eax
    orl     $0x01,%eax
    movl    %eax,%cr0
    /*这个地方比较重要，这条指令将cr0的pe位打开之后，下面的代码就是32位的*/
    ljmp    $(2*8),$protcseg

.code32
protcseg:
    movw    $(1*8),%ax
    /*初始化段寄存器*/
    movw    %ax,%ds
    movw    %ax,%es
    movw    %ax,%fs
    movw    %ax,%gs
    movw    %ax,%ss
    
    movl    $start,%esp
/*将软盘的内容复制到0x100000内存地址处*/
    movl    $DSKCAC0,%esi
    movl    $DSKCAC ,%edi
    movl    $0,%ecx
    movb    $6,%cl
    imul    $(512*63/4),%ecx
    subl    $(512/4),%ecx
    call    memcpy
/* 将C语言代码复制到 0x280000地址 */
    movl    $main,%esi
    movl    $0x280000,%edi
    movl    $(512*1024/4),%ecx
    call    memcpy

    /*双字跳转，跳转到0x280000:0x00000000*/
    ljmp    $(3*8),$0x0000
    jmp     .
memcpy:
    movl    (%esi),%eax
    addl    $4,%esi
    movl    %eax,(%edi)
    addl    $4,%edi
    subl    $1,%ecx
    jnz     memcpy
    ret
gdt:
    /* null 字段，使用0填充*/
    .word   0x0000,0x0000,0x0000,0x0000     
    /* 数据区域 */
    .word   0xffff,0x0000,0x9200,0x00cf
    /* 进入区域代码 */
    .word   0xffff,0x0000,0x9a00,0x0047
    /* C 语言代码 */
    .word   0xffff,0x0000,0x9a28,0x0047
    /* 此区域不使用 */
    .word   0x00
gdtdesc:
    /*sizeof(gdt) -1 */
    .word   31      
    /* gdt 地址*/
    .long   gdt     
main:


    .org    510
    .word   0xaa55


