.text
.code16
.set BOTPAK, 0x00280000 # 加载bootpack文件的目标位置
.set DSKCAC, 0x00100000 # 磁盘缓存位置
.set DSKCAC0,0x00008000 # 实模式下磁盘缓存位置

# 启动信息
.set CYLS,0x0ff0
.set LEDS,0x0ff1        # 设置键盘LED指示灯
.set VMODE,0x0ff2       # 设置显示器颜色数目
.set SCRANX,0x0ff4      # 设置显示器的X坐标
.set SCRANY,0x0ff6      # 设置显示器的Y坐标
.set VRAM,0x0ff8        # 设置显存首地址

    # 设置显示模式
    movb    $0x13,%al       # 320*200*8bit 显示器
    movb    $0x00,%ah
    int     $0x10

    # 保存显示器信息
    movb    $8,(VMODE)
    movw    $320,(SCRANX)
    movw    $200,(SCRANY)
    movl    $0x000a0000,(VRAM)

    # 从BIOS加载键盘LED的信息
    movb    $0x02,%ah
    int     $0x16
    movb    %al,(LEDS)

    # AT兼容机的规格中，PIC的初始化需要在CLI前进行
    movb    $0xff,%al
    outb    %al,$0x21
    nop 
    outb    %al,$0xa1
    
    cli                     # 屏蔽所有的CPU中断信息
    # 设置A20GATE,允许访问1MB以上的内存
    call    waitkbout
    movb    $0xd1,%al
    outb    %al,$0x64
    call    waitkbout
    movb    $0xdf,%al
    outb    %al,$0x60
    call    waitkbout

# 保护模式，进入i486汇编模式
.arch   i486
    lgdt    (GDTR0)             # 指定GDT
    movl    %cr0,%eax
    andl    $0x7fffffff,%eax
    orl     $0x00000001,%eax
    movl    %eax,%cr0
    jmp     pipeline_flush
pipeline_flush:
    # 初始化寄存器
    movw    $1*8,%ax
    movw    %ax,%ds
    movw    %ax,%es
    movw    %ax,%fs
    movw    %ax,%gs
    movw    %ax,%ss

# 传送到bootpack地址处，将bootpack处的二进制指令搬运到这里
    movl    $bootpack,%esi
    movl    $BOTPAK,%edi
    movl    $512*1024/4,%ecx
    call    memcpy
# 将磁盘数据传输到原始位置
# 引导扇区
    movl    $0x7c00,%esi
    movl    $DSKCAC,%edi
    movl    $512/4,%ecx
    call    memcpy
# 将剩下全部的复制到对应的内存地址当中
    movl    $DSKCAC0+512,%esi
    movl    $DSKCAC+512,%edi
    movl    $0, %ecx
    movb    (CYLS), %cl
    imull   $512*18*2/4,%ecx
    sub     $512/4,%ecx
    call    memcpy
# bootpack启动位置
    movl    $BOTPAK,%ebx
    movl    16(%ebx),%ecx
    add     $3,%ecx
    shr     $2,%ecx             # 相当于(ecx+3)/4
    jz      skip
    movl    20(%ebx),%esi
    add     %ebx,%esi
    movl    12(%ebx),%edi
    call    memcpy

skip:
    mov     12(%ebx),%esp       # 初始化栈顶指针
    ljmp    $2*8,$0x0000001b    # 双字跳转

waitkbout:
    inb     $0x64,%al
    andb    $0x02,%al
    jnz     waitkbout
    ret

memcpy:
    movl    (%esi),%eax
    add     $4,%esi
    movl    %eax,(%edi)
    add     $4,%edi
    sub     $1,%ecx
    jnz     memcpy
    ret

.align 16
GDT0:
    .skip 8, 0x00
    .word 0xffff, 0x0000, 0x9200, 0x00cf
    .word 0xffff, 0x0000, 0x9a28, 0x0047
    .word 0x0000
GDTR0:
    .word 8*3-1
    .int GDT0
bootpack:
