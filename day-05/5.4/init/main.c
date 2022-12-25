#include "utils/sprintf.h"
#include "utils/head.h"
#include "utils/gdtidt.h"

void boot_main(void){
    char *vram;
    char str[32] = {0};
    char mcursor[16 * 16];
    int mx,my;
    
    struct BootInfo *binfo = (struct BootInfo *) 0xff0;
    /*初始化GDT、IDT表*/
    init_gdtidt();
    
    /*初始化画板*/
    init_palette();
    /*初始化桌面*/
    init_screen(binfo->vram, binfo->scranx, binfo->scrany);
    /*初始化指针*/
    init_mouse_cursor8(mcursor, COL8_008484);
    mx = (binfo->scranx - 16) / 2;
    my = (binfo->scrany - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scranx, 16, 16, mx, my, mcursor, 16);
    
    sprintf(str, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scranx, 0, 0, COL8_FFFFFF, str);
    
    sprintf(str, "scranx = %d", binfo->scranx);
    putfonts8_asc(binfo->vram, binfo->scranx, 16, 64, COL8_FFFFFF, str);
   
    putfonts8_asc(binfo->vram, binfo->scranx, 31, 31, COL8_000000, "Hello OS.");
    putfonts8_asc(binfo->vram, binfo->scranx, 30, 30, COL8_FFFFFF, "Hello OS.");
    
    for(;;){
        io_hlt();
    }
}
