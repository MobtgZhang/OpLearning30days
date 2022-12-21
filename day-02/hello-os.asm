;hello-s
;TAB=4
    org 0x7c00              ;BIOS指明程序装载地址
; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code
    jmp entry
    db  0x90
    db  "HELLOIPL"		; 启动扇区名称（8字节）
	dw  512				; 每个扇区（sector）大小（必须512字节）
	db  1				; 簇（cluster）大小（必须为1个扇区）
	dw	1				; FAT起始位置（一般为第一个扇区）
    db	2				; FAT个数（必须为2）
    dw	224				; 根目录大小（一般为224项）
    dw	2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
    db	0xf0			; 磁盘类型（必须为0xf0）
    dw	9				; FAT的长度（必??9扇区）
    dw	18				; 一个磁道（track）有几个扇区（必须为18）
    dw	2				; 磁头数（必??2）
    dd	0				; 不使用分区，必须是0
    dd	2880			; 重写一次磁盘大小
    db	0,0,0x29		; 意义不明（固定）
    dd	0xffffffff		; （可能是）卷标号码
    db	"HELLO-OS   "	; 磁盘的名称（必须为11字?，不足填空格）
    db	"FAT12   "		; 磁盘格式名称（必??8字?，不足填空格）
    RESB    18				; 先空出18字节
;程序主体
entry:
    mov ax,0
    mov ss,ax
    mov sp,0x7c00
    mov ds,ax
    mov es,ax

    mov si,msg
putloop:
    mov al,[si]
    add si,1
    cmp al,0
    je fin
    mov ah,0x0e                 ;显示一个文字
    mov bx,15                   ;指定字符颜色
    int 0x10                    ;调用显卡bios
    jmp putloop
fin:
    hlt                         ;让CPU停止，等待指令
    jmp fin
;定义显示输出的内容
msg:
    db  0x0a,0x0a               ;两次换行处理
    db  "hello,world!"          
    db  0x0a                    ;换行处理
    db  0
    TIMES 510-($-$$) DB 0x00    ;自动填写0x00直到0x001fe
    db      0x55,0xaa

