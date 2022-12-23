; hello-os
; TAB=4
; 标准的FAT12软盘专用的代码
    DB  0xeb,0x4e,0x90
    DB  "HELLOIPL"              ;定义了启动扇区的名称,这里占用了8字节
    DW  512                     ;每个扇区(sector)大小,这里必须为512字节
    DB  1                       ;簇(cluster)大小,这里必须为一个扇区
    DW  1                       ;FAT起始位置(一般是第一个扇区)
    DB  2                       ;FAT个数，必须为2
    DW  224                     ;根目录的大小，一般为224项
    DW  2880                    ;该磁盘的大小，必须为2880扇区，1440*1024/512=2880
    DB  0xf0                    ;磁盘类型，必须为0xf0
    DW  9                       ;FAT的长度，必须是9扇区
    DW  18                      ;一个磁道(track)当中有几个扇区，必须为18
    DW  2                       ;磁头数量，必须为2
    DD  0                       ;这里不使用分区，必须是0
    DD  2880                    ;重写一次磁盘大小
    DB  0,0,0x29                ; 意义不明，固定写法
    DD  0xffffffff              ;卷标号码
    DB  "HELLO-OS   "           ;磁盘名称，必须为11字节，不足直接填写空格
    DB  "FAT12   "              ;磁盘格式的名称，必须为8字节，不足直接填写空格
    RESB    18                  ;这里先空出18字节

;这里是程序的主体
    DB  0xb8, 0x00, 0x00, 0x8e, 0xd0, 0xbc, 0x00, 0x7c
	DB  0x8e, 0xd8, 0x8e, 0xc0, 0xbe, 0x74, 0x7c, 0x8a
	DB  0x04, 0x83, 0xc6, 0x01, 0x3c, 0x00, 0x74, 0x09
	DB  0xb4, 0x0e, 0xbb, 0x0f, 0x00, 0xcd, 0x10, 0xeb
	DB  0xee, 0xf4, 0xeb, 0xfd

;信息显示的部分
    DB  0x0a,0x0a           ;这里表示两个换行
    DB  "hello,world!"      ;
    DB  0x0a                ;表示的是换行操作符
    DB  0

    RESB    0x1fe-($-$$)			; 填写0x00直到0x001fe

    DB  0x55,0xaa
;下面是启动区以外部分的输出

    DB  0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
	RESB    4600
	DB  0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
	RESB    1469432
