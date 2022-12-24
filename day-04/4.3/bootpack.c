# include"head.h"
void boot_main(void){
    char*ptr;
    init_palette();
    ptr = (char*) 0xa0000;
    boxfill8(ptr,320,COL8_FF0000,20,20,120,120);    
    boxfill8(ptr,320,COL8_00FF00,70,50,170,150);
    boxfill8(ptr,320,COL8_00FFFF,120,80,220,180);
    for(;;){
        io_hlt();
    }
}
