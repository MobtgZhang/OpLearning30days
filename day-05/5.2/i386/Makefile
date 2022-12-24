ipl_link_script=ipl.lds
os_link_script=os.lds
bootpack_link_script=bootpack.lds
bootpack_func=boot_main

ipl_src_file=ipl.S
os_src_file=boot.S
bootpack_src_file=bootpack.c
sprintf_src_file=sprintf.c
asm_lib_src=asm_funcs.S
font_src_file=fonts.S


target_dir=build
ipl_bin_file=$(target_dir)/ipl.bin
os_bin_file=$(target_dir)/boot.bin
bootpack_bin_file=$(target_dir)/bootpack.bin

asm_lib_obj_file=$(target_dir)/asm_funcs.o
os_obj_file=$(target_dir)/boot.o
ipl_obj_file=$(target_dir)/ipl.o
bootpack_obj_file=$(target_dir)/bootpack.o
sprintf_obj_file=$(target_dir)/sprintf.o
font_bin_file=$(target_dir)/fonts.o


system_img_file=$(target_dir)/system.img
target_img_file=$(target_dir)/boot.img

#debug
ipl_lst_file=$(target_dir)/ipl.lst
os_lst_file=$(target_dir)/os.lst
asm_lib_lst_file=$(target_dir)/asm_func.lst
font_lst_file=$(target_dir)/fonts.lst
QEMU=qemu-system-x86_64

all:$(target_img_file)
build-dir:
	mkdir -p $(target_dir)

$(os_bin_file): build-dir $(os_src_file) $(os_link_script) 
	gcc -nostdlib -o $@ -T $(os_link_script) $(os_src_file)
	gcc -T $(os_link_script) -c -g -Wa,-a,-ad $(os_src_file) -o $(os_obj_file) >$(os_lst_file)
$(font_bin_file): build-dir $(font_src_file)
	gcc -m32 -T .data -c -g -Wa,-a,-ad $(font_src_file) -o $@ > $(font_lst_file)


$(ipl_bin_file): build-dir $(ipl_src_file) $(ipl_link_script)
	gcc -nostdlib -o $@ -T$(ipl_link_script) $(ipl_src_file)
	gcc -T $(ipl_link_script) -c -g -Wa,-a,-ad $(ipl_src_file) -o $(ipl_obj_file) > $(ipl_lst_file)
$(bootpack_bin_file):build-dir $(bootpack_src_file) $(asm_lib_obj_file) $(sprintf_src_file) $(font_bin_file)
	gcc -nostdlib -m32 -c -o $(bootpack_obj_file) $(bootpack_src_file)
	gcc -nostdlib -m32 -c -o $(sprintf_obj_file) $(sprintf_src_file)
	ld -m elf_i386 -o $@ -T $(bootpack_link_script) -e $(bootpack_func) --oformat=binary $(bootpack_obj_file) $(asm_lib_obj_file) $(font_bin_file) $(sprintf_obj_file)
$(asm_lib_obj_file): build-dir $(asm_lib_src)
	gcc -m32 -c -g -Wa,-a,-ad $(asm_lib_src) -o $(asm_lib_obj_file) > $(asm_lib_lst_file)


$(system_img_file): $(os_bin_file) $(bootpack_bin_file)
	cat $(os_bin_file) $(bootpack_bin_file) > $@
$(target_img_file):$(system_img_file) $(ipl_bin_file)
	mformat -f 1440 -B $(ipl_bin_file) -C -i $(target_img_file) ::
	mcopy $(system_img_file) -i $(target_img_file) ::

run:$(target_img_file)
	$(QEMU) -m 32 -drive format=raw,file=$(target_img_file),if=floppy

debug:all
	$(QEMU) -drive format=raw,file=$(target_img_file),if=floppy -gdb tcp::10000 -S

clean:
	rm -rf $(target_dir)
