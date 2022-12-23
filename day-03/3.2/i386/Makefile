AS=gcc
QEMU=qemu-system-i386
ipl_src_file=ipl.s
ipl_link_script=ipl.lds

os_src_file=hello-os.s
os_link_script=hello-os.lds


des_dir=bin
ipl_bin_file=$(des_dir)/ipl.bin
ipl_obj_file=$(des_dir)/ipl.o
os_bin_file=$(des_dir)/hello-os.bin
os_obj_file=$(des_dir)/hello-os.o
target_img_file=$(des_dir)/boot.img

# debug list
ipl_list_file=$(des_dir)/ipl.lst
os_list_file=$(des_dir)/hello-os.lst

all:$(target_img_file)

$(os_bin_file):$(os_src_file) $(os_link_script)
	mkdir -p $(des_dir)
	gcc -nostdlib -o $@ -T$(os_link_script) $(os_src_file)
	gcc -T $(os_link_script) -c -g -Wa,-a,-ad $(os_src_file) -o $(os_obj_file) > $(os_list_file)

$(ipl_bin_file):$(ipl_src_file) $(ipl_link_script)
	mkdir -p $(des_dir)
	gcc -nostdlib -o $@ -T$(ipl_link_script) $(ipl_src_file)
	gcc -T $(ipl_link_script) -c -g -Wa,-a,-ad $(ipl_src_file) -o $(ipl_obj_file) > $(ipl_list_file)
$(target_img_file): $(os_bin_file) $(ipl_bin_file)
	mformat	-f 1440 -B $(ipl_bin_file) -C -i $(target_img_file) ::
	mcopy $(os_bin_file) -i $(target_img_file) ::
run:all
	$(QEMU) -m 32 -drive format=raw,file=$(target_img_file),if=floppy
debug:all
	$(QEMU) -m 32 -drive format=raw,file=$(target_img_file),if=floppy -gdb tcp::10000 -S
clean:
	rm -rf $(des_dir)

