#ifndef GDTIDT_HEAD_H
#define GDTIDT_HEAD_H
/*段描述结构体*/
struct SegDescriptor{
    short limit_low,base_low;
    char base_mid,access_right;
    char limit_high,base_high;
};
struct GateDescriptor{
    short offset_low,selector;
    char dw_count,access_right;
    short offset_high;
};
/*初始化GDT、IDT表*/
void init_gdtidt(void);
/*设置段表描述符 GDT*/
void set_segmdesc(struct SegDescriptor * sd,unsigned int limit,int base,int ar);
/*设置GAT*/
void set_gatedesc(struct GateDescriptor* gd,int offset,int selector,int ar);
#endif
