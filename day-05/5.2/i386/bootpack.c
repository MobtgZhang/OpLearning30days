#include"head.h"
#include"sprintf.h"
void boot_main(){
    char* vram;
    char str_line[32] = {0};
    struct BootInfo *binfo = (struct BootInfo *) 0xff0;
    init_palette();
    init_screen(binfo->vram,binfo->scranx,binfo->scrany);
    sprintf(str_line,"scranx=%d",binfo->scranx);
    putfonts8_asc(binfo->vram, binfo->scranx, 16, 64, COL8_FFFFFF, str_line);
    putfonts8_asc(binfo->vram, binfo->scranx, 8, 8, COL8_FFFFFF, "ABC 123");
    putfonts8_asc(binfo->vram, binfo->scranx, 31, 31, COL8_000000, "Hello OS.");
    putfonts8_asc(binfo->vram, binfo->scranx, 30, 30, COL8_FFFFFF, "Hello OS.");
    for(;;){
        io_hlt();
    }
}
