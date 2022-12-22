# include<head.h>
void bootmain(){
    /*显存的起始位置*/
    int i= 0xa0000;
    for(;i<=0xaffff;i++){
        *((char*)i)=15;
    }
    init_palette();
    char* ptr = 0xa0000;
    for(i=0;i<=0xffff;i++){
        *(ptr+i)=i&0xf;
    }
    clear_screen(15);
    draw_window();
    while(1);
}
