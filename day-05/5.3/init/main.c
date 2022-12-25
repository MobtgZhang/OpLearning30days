# include "utils/head.h"
# include "utils/sprintf.h"
void boot_main(void){
    char* vram;
    char str_line[32] = {0};
    char mcursor[16*16];
    /*记录鼠标的位置*/
    int mx,my;
    struct BootInfo *binfo = (struct BootInfo *) 0xff0;
    /*初始化调色板*/
    init_palette();
    /*初始化桌面*/
    init_screen(binfo->vram,binfo->scranx,binfo->scrany);
    /*初始化鼠标*/
    init_mouse_cursor8(mcursor,COL8_008484);
    /*鼠标的位置*/
    mx = (binfo->scranx - 16)/2;
    my = (binfo->scrany - 28 - 16)/2;
    putblock8_8(binfo->vram,binfo->scranx,16,16,mx,my,mcursor,16);

    sprintf(str_line, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scranx, 0, 0, COL8_FFFFFF, str_line);
    
    sprintf(str_line, "scranx = %d", binfo->scrany);
    putfonts8_asc(binfo->vram, binfo->scrany, 16, 64, COL8_FFFFFF, str_line);
   
    putfonts8_asc(binfo->vram, binfo->scrany, 31, 31, COL8_000000, "Hello OS.");
    putfonts8_asc(binfo->vram, binfo->scrany, 30, 30, COL8_FFFFFF, "Hello OS.");
    
}

