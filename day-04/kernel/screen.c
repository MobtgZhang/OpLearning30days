#include<head.h>
#include<x86.h>
/*用于清空屏幕的操作*/
void clear_screen(char color){
    int i;
    for(i=0xa0000;i<0xaffff;i++){
        *(char*)i=color;
    }
}
/*用于彩色屏幕显示颜色*/
void color_screen(char color){
    int i;
    for(i=0xa0000;i<0xaffff;i++){
        *(char*)i=i;
    }
}
/*初始化调色板，其中数组table_rgb[]保存了16种颜色编码*/
/*x86处理器设置端口0x03c8 0x03c9*/
void init_palette(void){
    //16种color，每个color三个字节。
    unsigned char table_rgb[16*3]={
        0x00,0x00,0x00,   /*0:black*/
        0xff,0x00,0x00,   /*1:light red*/
        0x00,0xff,0x00,   /*2:light green*/
        0xff,0xff,0x00,   /*3:light yellow*/

        0x00,0x00,0xff,   /*4:light blue*/
        0xff,0x00,0xff,   /*5:light purper*/
        0x00,0xff,0xff,   /*6:light blue*/
        0xff,0xff,0xff,   /*7:white*/

        0xc6,0xc6,0xc6,   /*8:light gray*/
        0x84,0x00,0x00,   /*9:dark red*/
        0x00,0x84,0x00,   /*10:dark green*/
        0x84,0x84,0x00,   /*11:dark yellow*/

        0x00,0x00,0x84,   /*12:dark 青*/
        0x84,0x00,0x84,   /*13:dark purper*/
        0x00,0x84,0x84,   /*14:light blue*/
        0x84,0x84,0x84,   /*15:dark gray*/
    };
    set_palette(0,255,table_rgb);
}

/*设置调色板，这里仅仅使用到了16个颜色，后面并没有用到*/
void set_palette(int start,int end, unsigned char *rgb){
    int i,eflag;
    /*记录从前的cpsr值*/
    eflag=read_eflags(); 

    io_cli(); 
    outb(0x03c8,start);
    /*outb函数是往指定的设备，送数据。*/
    for(i=start;i<=end;i++)
    {
        outb(0x03c9,*(rgb)/4);    
        outb(0x03c9,*(rgb+1)/4);
        outb(0x03c9,*(rgb+2)/4);
        rgb=rgb+3;
    }
    /*恢复之前的cpsr*/
    write_eflags(eflag);
    return;
}

void boxfill8(unsigned char *vram,int xsize,unsigned char color,int x0,int y0,int x1,int y1){
    int x,y;
    for(y=y0;y<=y1;y++){
        for(x=x0;x<=x1;x++){
            vram[y*xsize+x]=color;
        }
    }

}
void boxfill(unsigned char color,int x0,int y0,int x1,int y1){
    boxfill8((unsigned char *)VRAM,320,color,x0,y0,x1,y1);
}

/*画出窗口函数*/
void draw_window(){
    unsigned char *p;
    int x=320,y=200;
    p=(unsigned char*)VRAM;
    /*画出一个窗口*/
    boxfill(7 ,0, 0   ,x-1,y-29);
    /*画出一个按键*/
    boxfill(8  ,0, y-28,x-1,y-28);
    boxfill(7  ,0, y-27,x-1,y-27);
    boxfill(8  ,0, y-26,x-1,y-1);
    /*左边按键*/
    boxfill(7, 3,  y-24, 59,  y-24);
    boxfill(7, 2,  y-24, 2 ,  y-4);
    boxfill(15, 3,  y-4,  59,  y-4);
    boxfill(15, 59, y-23, 59,  y-5);
    boxfill(0, 2,  y-3,  59,  y-3);
    boxfill(0, 60, y-24, 60,  y-3);
    /*右边按键*/
    boxfill(15, x-47, y-24,x-4,y-24);
    boxfill(15, x-47, y-23,x-47,y-4);
    boxfill(7, x-47, y-3,x-4,y-3);
    boxfill(7, x-3, y-24,x-3,y-3);
}

/*显示鼠标*/
void init_mouse(char *mouse,char bg){
    #define background 7
    #define outline    0
    #define inside     2

    const static char cursor[16][16] = {
            "**************..",
            "*OOOOOOOOOOO*...",
            "*OOOOOOOOOO*....",
            "*OOOOOOOOO*.....",
            "*OOOOOOOO*......",
            "*OOOOOOO*.......",
            "*OOOOOOO*.......",
            "*OOOOOOOO*......",
            "*OOOO**OOO*.....",
            "*OOO*..*OOO*....",
            "*OO*....*OOO*...",
            "*O*......*OOO*..",
            "**........*OOO*.",
            "*..........*OOO*",
            "............*OO*",
            ".............***"
    };
	int x,y;
	for(y=0;y<16;y++){
	  for(x=0;x<16;x++){
	    switch (cursor[y][x]){
	      case '.':mouse[x+16*y]=bg;break;  
	      case '*':mouse[x+16*y]=outline;break;
	      case 'O':mouse[x+16*y]=inside;break;
	    }
	  }
	}
}

/*显示鼠标*/
void display_mouse(char *vram,int xsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize){
    int x,y;
    for(y=0;y<pysize;y++){
        for(x=0;x<pxsize;x++){
        vram[(py0+y)*xsize+(px0+x)]=buf[y*bxsize+x];
        }
    }
}
