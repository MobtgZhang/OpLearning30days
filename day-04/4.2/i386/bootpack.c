/*用于CPU停机的操作*/
void io_hlt(void);
/*用于关中断操作*/
void io_cli(void);
/*用于给某个端口写数据*/
void io_out8(int port,int data);
/*用于加载和修改EFLAGS寄存器的函数*/
int io_load_eflags(void);
void io_store_eflags(void);
/*初始化调色板以及修改颜色参数*/
void init_palette(void);
void set_palette(int start,int end,unsigned char *rgb);

void boot_main(){
    int i;
    char *p;
    init_palette();
    p = (char*) 0xa0000;
    for(i=0;i<=0xffff;i++){
        *(p+i)=i & 0x0f;
    }
    for(;;){
        io_hlt();
    }
}
void init_palette(void){
    static unsigned char rgb_table[16*3] = {
        0x00,0x00,0x00, /*1. 黑色*/
        0xff,0x00,0x00, /*2. 红色*/
        0x00,0xff,0x00, /*3. 绿色*/
        0x00,0x00,0xff, /*4. 蓝色*/
        0xff,0xff,0x00, /*5. 黄色*/
        0xff,0x00,0xff, /*6. 紫色*/
        0x00,0xff,0xff, /*7. 浅蓝*/
        0xff,0xff,0xff, /*8. 白色*/
        0xc6,0xc6,0xc6, /*9. 灰色*/
        0x84,0x00,0x00, /*10. 暗红*/
        0x00,0x84,0x00, /*11. 暗绿*/
        0x00,0x00,0x84, /*12. 暗蓝*/
        0x84,0x84,0x00, /*13. 暗黄*/
        0x00,0x84,0x84, /*14. 暗浅蓝*/
        0x84,0x00,0x84, /*15. 暗紫*/
        0x84,0x84,0x84, /*16. 暗灰*/
    };
    set_palette(0,15,rgb_table);
    return ;
}
void set_palette(int start,int end,unsigned char * rgb){
    int i,eflags;
    eflags = io_load_eflags();
    /*将中断标志设置位0，屏蔽中断*/
    io_cli();
    io_out8(0x03c8,start);
    for(i=start;i<=end;i++){
        io_out8(0x03c9,rgb[0] /4);
        io_out8(0x03c9,rgb[1] /4);
        io_out8(0x03c9,rgb[2] /4);
        rgb += 3;
    }
    /*恢复标志位*/
    io_store_eflags();
    return ;
}
