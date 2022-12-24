/*定义一些颜色*/
#ifndef COLOR_H
#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_0000FF 3
#define COL8_00FFFF 4
#define COL8_FF00FF 5
#define COL8_FFFF00 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_000084 11
#define COL8_008484 12
#define COL8_840084 13
#define COL8_848400 14
#define COL8_848484 15
#endif
/*定义一些ASM函数*/
#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H
/*用于CPU停机的操作*/
void io_hlt(void);
/*用于关中断操作*/
void io_cli(void);
/*用于给某个端口写数据*/
void io_out8(int port,int data);
/*用于加载和修改EFLAGS寄存器的函数*/
int io_load_eflags(void);
void io_store_eflags(void);
#endif

#ifndef HEAD_H
#define HEAD_H
/*设置调色板*/
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
/*初始化调色板*/
void init_palette(void){
    static unsigned char rgb_table[16*3] = {
        0x00,0x00,0x00, /*1. 黑色*/
        0xff,0x00,0x00, /*2. 红色*/
        0x00,0xff,0x00, /*3. 绿色*/
        0x00,0x00,0xff, /*4. 蓝色*/
        0x00,0xff,0xff, /*5. 浅蓝*/
        0xff,0x00,0xff, /*6. 紫色*/
        0xff,0xff,0x00, /*7. 黄色*/
        0xff,0xff,0xff, /*8. 白色*/
        0xc6,0xc6,0xc6, /*9. 灰色*/
        0x84,0x00,0x00, /*10. 暗红*/
        0x00,0x84,0x00, /*11. 暗绿*/
        0x00,0x00,0x84, /*12. 暗蓝*/
        0x00,0x84,0x84, /*13. 暗浅蓝*/
        0x84,0x00,0x84, /*14. 暗紫*/
        0x84,0x84,0x00, /*15. 暗黄*/
        0x84,0x84,0x84, /*16. 暗灰*/
    };
    set_palette(0,15,rgb_table);
    return ;
}
/*绘制矩形*/
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    int x, y;
    for(y = y0; y <= y1; y++){
        for(x = x0; x <= x1; x++){
            vram[y * xsize + x] = c;
        }
    }
}
#endif


