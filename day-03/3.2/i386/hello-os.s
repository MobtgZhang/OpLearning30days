.text
.code16

.set CYLS,0x0ff0
.set LEDS,0x0ff1
.set VMODE,0x0ff2
.set SCRANX,0x0ff4
.set SCRANY,0x0ff6
.set VRAM,0x0ff8

# 设置VGA图形显示320*200*8bit
    movb        $0x13,%al
    movb        $0x00,%ah
    int         $0x10

    # 保存显示器信息
    movb        $8,   (VMODE)
    movw        $320, (SCRANX)
    movw        $200, (SCRANY)
    movl        $0x000a0000, (VRAM)
    # 从BIOS获取LED指示灯BIOS
    mov         $0x02,%ah
    int         $0x16
    mov         %al,(LEDS)
finish:
    hlt
    jmp         finish
