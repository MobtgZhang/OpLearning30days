;haribote.asm
;TAB=4
CYLS        equ     0x0ff0              ;设定启动区
LEDS        equ     0x0ff1              
VMODE       equ     0x0ff2              ;关于颜色数目的信息，颜色的位数
SCARNX      equ     0x0ff4              ;分辨率的X
SCARNY      equ     0x0ff6              ;分辨率的Y
VRAM        equ     0x0ff8              ;图像缓冲区的起始地址

            org     0xc200              ;程序装载位置
            mov     al,0x13             ;VGA显卡，用于320x200x8位彩色显示器
            mov     ah,0x00
            int     0x10
            mov     byte[VMODE],8       ;记录画面模式
            mov     word[SCARNX],320
            mov     word[SCARNY],200
            mov     dword[VRAM],0x000a000
;用BIOS取得键盘上各种LED灯当前的状态信息
            mov     ah,0x02
            int     0x16
            mov     [LEDS],al
            times   510-($-$$)  db  0x00
            db      0x55,0xaa

fin:
            hlt
            jmp     fin