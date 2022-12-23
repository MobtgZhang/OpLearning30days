ipl_link_script=ipl.lds
os_link_script=os.lds
bootpack_link_script=bootpack.lds

ipl_src_file=ipl.S
os_src_file=hello.S
bootpack_src_file=bootpack.c

target_dir=bin
ipl_bin_file=$(target_dir)/ipl.bin
os_bin_file=$(target_dir)/hello.bin
bootpack_bin_file=$(target_dir)/bootpack.bin

os_obj_file=$(target_dir)/hello.o
ipl_obj_file=$(target_dir)/ipl.o
system_img_file=$(target_dir)/system.img
target_img_file=$(target_dir)/hello.img

# debug files

ipl_lst_file=$(target_dir)/ipl.lst
os_lst_file=$(target_dir)/hello.lst

QEMU=qemu-system-i386

all:$(target_img_file)

pre_make:mkdir -p $(target_dir)

$(os_bin_file): $(os_src_file) $(os_link_script) pre_make
	gcc -nostdlib -o $@ -T $(os_link_script) $(os_src_file)
	gcc -T $(os_link_script) -c -g -Wa,-a,-ad $(os_src_file) -o > $(os_lst_file)

$(ipl_bin_file): $(ipl_src_file) $(ipl_link_script) pre_make
	gcc -nostdlib -o $@ -T $(ipl_link_script) $(ipl_src_file)
	gcc -T $(ipl_link_script) -c -g -Wa,-a,-ad $(ipl_src_file) -o > $(ipl_lst_file)
$(bootpack_bin_file):$(bootpack_src_file) pre_make
	gcc -nostdlib -m32 -T $(bootpack_link_script) -o $@ $(bootpack_src_file)

$(system_img_file): $(os_bin_file) $(ipl_bin_file)
	cat $(os_bin_file) $(bootpack_bin_file) > $@
$(target_img_file): $(system_img_file) $(ipl_bin_file)
	mformat -f 1440 -B $(ipl_bin_file) -C -i $(target_img_file) ::
	mcopy $(system_img_file) -i $(target_img_file) ::
run:all
	$(QEMU) -m 32 -drive format=raw,file=$(target_img_file),if=floppy
debug:all
	$(QEMU) -drive format=raw,file=$(TARGET_IMG),if=floppy -gdb tcp::10000 -S
clean:
	rm -rf $(target_dir)
