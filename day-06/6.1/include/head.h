#ifndef COLOR_H
#define COLOR_H
/*定义颜色类型*/
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

/*定义一些原子操作*/
#ifndef ASM_ATOM_HEAD_H
#define ASM_ATOM_HEAD_H
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
#endif

/*定义一些画图函数*/
#ifndef PALETTE_HEAD_H
#define PALETTE_HEAD_H
void init_palette(void);
void init_screen(char *vram, int xsize, int ysize);

void set_palette(int start, int end, unsigned char *rgb);

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

/*定义启动参数*/
struct BootInfo{
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short scranx;
    short scrany;
    char *vram;
};
#endif
