# 第01天
## 主要的一些术语
+ **FAT12格式**：它是用于Windows或者是MS-DOS操作系统使用到的一种文件系统，磁盘主要结构如下所示：

  + **引导扇区(Boot Sector)**：位于第一个扇区，在软盘上就0柱面，0磁道，1扇区的位置；
  + **文件分配表(FAT)**： 紧接着引导扇区的是两个完全相同的FAT表，每个FAT表占用9个扇区；
  + **根目录区**：FAT表之后就是根目录区域，一般长度不固定；
  + **数据区**：位置在根目录之后。

### 引导扇区格式
引导扇区格式如下所示：
<center>
    <table>
        <tr>
            <td>名称</td>
            <td>偏移</td>
            <td>长度</td>
            <td>内容</td>
            <td>软盘参考值</td>
        </tr>
        <tr>
            <td>BS_jmpBoot</td>
            <td>0</td>
            <td>3</td>
            <td>无</td>
            <td>jmp LABEL_START <br></br> nop</td>
        </tr>
        <tr>
            <td>BS_OEMName</td>
            <td>3</td>
            <td>8</td>
            <td>厂商名称</td>
            <td>"HELLO-OS"（这里八个字节的字符串即可）</td>
        </tr>
        <tr>
            <td>BPB_BytesPerSec</td>
            <td>11</td>
            <td>2</td>
            <td>每扇区字节数</td>
            <td>0x200(十进制512)</td>
        </tr>
        <tr>
            <td>BPB_SecPerClus</td>
            <td>13</td>
            <td>1</td>
            <td>每簇扇区数</td>
            <td>0x01</td>
        </tr>
        <tr>
            <td>BPB_RsvdSecCnt</td>
            <td>14</td>
            <td>2</td>
            <td>Boot记录占用多少扇区</td>
            <td>0x01</td>
        </tr>
        <tr>
            <td>BPB_NumFATs</td>
            <td>16</td>
            <td>1</td>
            <td>一共有多少FAT表</td>
            <td>0x02</td>
        </tr>
        <tr>
            <td>BPB_RootEntCnt</td>
            <td>17</td>
            <td>2</td>
            <td>根目录文件数量最大值</td>
            <td>0xE0(224个)</td>
        </tr>
        <tr>
            <td>BPB_TotSec16</td>
            <td>19</td>
            <td>2</td>
            <td>扇区总数</td>
            <td>0xB40(2280个)</td>
        </tr>
        <tr>
            <td>BPB_Media</td>
            <td>21</td>
            <td>1</td>
            <td>介质描述符</td>
            <td>0xF0</td>
        </tr>
        <tr>
            <td>BPB_FATSz16</td>
            <td>22</td>
            <td>2</td>
            <td>每FAT扇区数</td>
            <td>0x09</td>
        </tr>
        <tr>
            <td>BPB_SecPerTrk</td>
            <td>24</td>
            <td>2</td>
            <td>每磁道扇区数</td>
            <td>0x12</td>
        </tr>
        <tr>
            <td>BPB_NumHeads</td>
            <td>26</td>
            <td>2</td>
            <td>磁头数</td>
            <td>0x02</td>
        </tr>
        <tr>
            <td>BPB_HiddSec</td>
            <td>28</td>
            <td>4</td>
            <td>隐藏扇区数</td>
            <td>0x00</td>
        </tr>
        <tr>
            <td>BPB_TotSec32</td>
            <td>32</td>
            <td>4</td>
            <td>如果BPB_TotSec16是0，这个记录扇区数量</td>
            <td>0xB40(2880)</td>
        </tr>
        <tr>
            <td>BS_DrvNum</td>
            <td>36</td>
            <td>1</td>
            <td>中断13的驱动器号</td>
            <td>0x00</td>
        </tr>
        <tr>
            <td>BS_Reserved1</td>
            <td>37</td>
            <td>1</td>
            <td>未使用</td>
            <td>0x00</td>
        </tr>
        <tr>
            <td>BS_BootSig</td>
            <td>38</td>
            <td>1</td>
            <td>扩展引导标记</td>
            <td>0x29</td>
        </tr>
        <tr>
            <td>BS_VolD</td>
            <td>39</td>
            <td>4</td>
            <td>卷序列号</td>
            <td>0x00</td>
        </tr>
        <tr>
            <td>BS_VolLab</td>
            <td>43</td>
            <td>11</td>
            <td>卷标</td>
            <td>"My Hello OS"</td>
        </tr>
        <tr>
            <td>BS_FileSysType</td>
            <td>54</td>
            <td>8</td>
            <td>文件系统类型</td>
            <td>"FAT12   "</td>
        </tr>
        <tr>
            <td>引导代码</td>
            <td>62</td>
            <td>448</td>
            <td>用于引导代码、数据以及其他填充字符等等</td>
            <td></td>
        </tr>
        <tr>
            <td>结束标志</td>
            <td>510</td>
            <td>2</td>
            <td></td>
            <td>0xAA55</td>
        </tr>
    </table>
</center>

### 文件分配表
考虑到系统冗余，FAT12包含有两份文件分配表，它是分区信息的映射表，指示簇是如何存储的；
每个FAT占用12bit，FAT项的值代表的是文件下一个簇号但是如果大于等于0xFF8，则表示当前簇已经是本文件的最后一个簇，如果值是0xFF7，则表示的是坏簇。

### 根目录区

它位于第二个FAT表之后，开始扇区号为19，由若干个目录条目组成，目录条目最多有`BPB_RootEntCnt`个，每个条目占用32字节，格式如下所示
<center>
    <table>
        <tr>
            <td>名称</td>
            <td>偏移</td>
            <td>长度</td>
            <td>描述</td>
        </tr>
        <tr>
            <td>DIR_Name</td>
            <td>0</td>
            <td>0xB</td>
            <td>文件8字节、扩展名3字节</td>
        </tr>
        <tr>
            <td>DIR_Attr</td>
            <td>0xB</td>
            <td>1</td>
            <td>文件属性</td>
        </tr>
        <tr>
            <td>保留</td>
            <td>0xC</td>
            <td>10</td>
            <td></td>
        </tr>
        <tr>
            <td>DIR_WrtTime</td>
            <td>0x16</td>
            <td>2</td>
            <td>最后修改时间</td>
        </tr>
        <tr>
            <td>DIR_WrtTime</td>
            <td>0x18</td>
            <td>2</td>
            <td>最后修改日期</td>
        </tr>
        <tr>
            <td>DIR_FstClus</td>
            <td>0x1A</td>
            <td>2</td>
            <td>此条目对应的开始簇号</td>
        </tr>
        <tr>
            <td>DIR_FileSize</td>
            <td>0x1C</td>
            <td>4</td>
            <td>文件大小</td>
        </tr>
    </table>
</center>

### 数据区
数据区域在根目录之后，数据区开始扇区号为根目录其实扇区号+根目录区域的大小，即

19+BPB_RootEntCnt*32/BPB_BytesPerSec

## 编译方式
笔者是在Deepin20.8 Linux 操作系统上进行编译，并且使用qemu运行。需要安装以下两个组件：

nasm编译器
```bash
sudo apt install nasm
```
qemu模拟器
```bash
sudo apt install qemu-system-x86
```

编译文件，但是`RESB`指令后面需要将填充字段`0x1fe-$`改成`0x1fe-($-$$)`
```bash
nasm hello-os.nas -o hello-os.img
```
运行模拟器
```bash
qemu-system-i386 -drive hello-os.img,if=floppy
```

