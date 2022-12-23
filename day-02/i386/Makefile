AS=gcc
QEMU=qemu-system-i386

link_script_file=link.lds
as_opt=-nostdlib -T $(link_script_file)
src_file=boot.s
target_dir=bin
target_img=$(target_dir)/boot.img
target_obj=$(target_dir)/boot.bin
target_size=1474560

all:img

img:asm
	dd if=$(target_obj) of=$(target_img) bs=1
	dd if=/dev/zero of=$(target_img) bs=1 count=0 seek=$(target_size)
asm:
	mkdir -p $(target_dir)
	$(AS) $(as_opt) $(src_file) -o $(target_obj)
$(target_img):all

run:$(target_img)
	$(QEMU) -drive format=raw,file=$(target_img),if=floppy
clean:
	rm -rf $(target_dir)

