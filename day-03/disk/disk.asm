;read disk 
;TAB=4
    CYLS    equ     10          ; 声明CYLS=10
    org     0x7c00              ; 指明程序装载地址

; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code
    jmp		entry
    db		0x90
    db		"HARIBOTE"		; 启动扇区名称（8字节）
    dw		512				; 每个扇区（sector）大小（必须512字节）
    db		1				; 簇（cluster）大小（必须为1个扇区）
    dw		1				; FAT起始位置（一般为第一个扇区）
    db		2				; FAT个数（必须为2）
    dw		224				; 根目录大小（一般为224项）
    dw		2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
    db		0xf0			; 磁盘类型（必须为0xf0）
    dw		9				; FAT的长度（必??9扇区）
    dw		18				; 一个磁道（track）有几个扇区（必须为18）
    dw		2				; 磁头数（必??2）
    dd		0				; 不使用分区，必须是0
    dd		2880			; 重写一次磁盘大小
    db		0,0,0x29		; 意义不明（固定）
    dd		0xffffffff		; （可能是）卷标号码
    db		"HARIBOTEOS "	; 磁盘的名称（必须为11字?，不足填空格）
    db		"FAT12   "		; 磁盘格式名称（必??8字?，不足填空格）
    resb	18				; 先空出18字节

;程序主体
entry:
    mov     ax,0            ;初始化寄存器
    mov     ss,ax
    mov     sp,0x7c00
    mov     ds,ax
;读取磁盘
    mov     ax,0x0820
    mov     es,ax
    mov     ch,0            ; 柱面0
    mov     dh,0            ; 磁头0
    mov     cl,2            ; 扇区2
readloop:
    mov     si,0            ; 记录失败次数寄存器
retry:
    mov     ah,0x02         ; ah=0x02, 读入磁盘
    mov     al,0x01         ; 1个扇区
    mov     bx,0
    mov     dl,0x00         ; A驱动器
    int     0x13            ; 调用磁盘中断int13
    jnc     next            ; 如果没有出错则跳转next

next:
    mov     ax,es           ; 将内存地址后移0x200
    add     ax,0x0020
    mov     es,ax
    add     cl,1
    cmp     cl,18           ; 判断是否读取到第18扇区的位置
    jbe     readloop        ; cl<=18 则跳转到readloop
    mov     cl,1
    add     dh,1
    cmp     dh,2
    jb      readloop        ; dh<2 则跳转到readloop
    mov     dh,0
    add     ch,1
    cmp     ch,CYLS
    jb      readloop        ; ch<cyls 则跳转到readloop，检查是否读取到了10个柱面

;这里读取完毕
    mov     [0x0ff0],ch     ; 查看ipl读取到什么程度
    jmp     0xc200
error:
    mov     si,msg
putloop:
    mov     al,[si]
    add     si,1
    cmp     al,0
    jmp     fin
    mov     ah,0x0e
    mov     bx,15
    int     0x10
    jmp     putloop
fin:
    hlt
    jmp     fin
msg:
    db      0x0a,0x0a
    db      "load error"
    db      0x0a
    db      0
    TIMES   510-($-$$)  db  0x00
    db      0x55,0xaa
