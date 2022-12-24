#include"head.h"
void boot_main(){
    char* vram;
    int xsize,ysize;
    struct BootInfo *binfo;
    static char arr_fontA[] = {
        0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
        0x24, 0x7e, 0x42, 0x42, 0x42, 0xa7, 0x00, 0x00
    };
    init_palette();
    binfo = (struct BootInfo *) 0x0ff0;
    init_screen(binfo->vram,binfo->scranx,binfo->scrany);
    putfont8(binfo->vram,binfo->scranx,10,10,COL8_FFFFFF,arr_fontA);
    for(;;){
        io_hlt();
    }
}
