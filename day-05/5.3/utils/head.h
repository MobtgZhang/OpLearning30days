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

#ifndef PALETTE_H
#define PALETTE_H
/*初始化调色板*/
void init_palette(void);
/*设置调色板*/
void set_palette(int start,int end,unsigned char * rgb);
/*绘制矩形*/
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
/*绘制字母*/
void putfont8(char* vram,int xsize,int x,int y, char c,char*font);
/*初始化屏幕*/
void init_screen(char *vram, int xsize, int ysize);
/*用于写入数字*/
void putfonts8_asc(char* varm,int xsize,int x,int y,char c,unsigned char *s);
/*初始化鼠标*/
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
/*定义了一些BIOS信息的结构体*/
struct BootInfo{
    char cyls,leds,vmode,reserve;
    short scranx,scrany;
    char *vram;
};
#endif
