ARCH=i386
QEMU=qemu-system-$(ARCH)
# the main function
bootpack_func=boot_main
# build directory
target_dir=build

# C source files
src_main_file=init/main.c
src_head_file=utils/head.c
src_sprintf_file=utils/sprintf.c

# S files
src_bootS_file=arch/$(ARCH)/boot.S
src_iplS_file=arch/$(ARCH)/ipl.S
src_fontS_file=arch/$(ARCH)/fonts.S
src_funcS_file=arch/$(ARCH)/asm_funcs.S

# lds files
boot_link_script=arch/$(ARCH)/boot.lds
ipl_link_script=arch/$(ARCH)/ipl.lds
main_link_script=arch/$(ARCH)/main.lds

# some binary files
bin_dir=$(target_dir)/$(ARCH)/bin
ipl_bin_file=$(bin_dir)/ipl.bin
boot_bin_file=$(bin_dir)/boot.bin
main_bin_file=$(bin_dir)/main.bin

# some object files
obj_dir=$(target_dir)/$(ARCH)/obj
obj_func_file=$(obj_dir)/asm_funcs.o
boot_obj_file=$(obj_dir)/boot.o
ipl_obj_file=$(obj_dir)/ipl.o
main_obj_file=$(obj_dir)/main.o
head_obj_file=$(obj_dir)/head.o
sprintf_obj_file=$(obj_dir)/sprintf.o
font_obj_file=$(obj_dir)/fonts.o

# img files
system_img_file=$(target_dir)/system-$(ARCH).img
target_img_file=$(target_dir)/boot-$(ARCH).img

# debug files
lst_dir=$(target_dir)/$(ARCH)/lst
main_lst_file=$(lst_dir)/main.lst
ipl_lst_file=$(lst_dir)/ipl.lst
obj_func_lst_file=$(lst_dir)/asm_func.lst
boot_lst_file=$(lst_dir)/boot.lst
font_lst_file=$(lst_dir)/fonts.lst
# the asm lib files
$(obj_func_file): $(src_funcS_file)
	mkdir -p $(obj_dir) $(lst_dir) $(bin_dir)
	gcc -m32 -c -g -Wa,-a,-ad $(src_funcS_file) -o $(obj_func_file) > $(obj_func_lst_file)
# the font files
$(font_obj_file): $(src_fontS_file)
	mkdir -p $(obj_dir) $(lst_dir) $(bin_dir)
	gcc -m32 -T .data -c -g -Wa,-a,-ad $(src_fontS_file) -o $@ >$(font_lst_file)
# the ipl files
$(ipl_bin_file): $(src_iplS_file) $(ipl_link_script)
	mkdir -p $(obj_dir) $(lst_dir) $(bin_dir)
	gcc -nostdlib -o $@ -T $(ipl_link_script) $(src_iplS_file)
	gcc -T $(ipl_link_script) -c -g -Wa,-a,-ad $(src_iplS_file) -o $(ipl_obj_file) > $(ipl_lst_file)
# the boot S files
$(boot_bin_file): $(src_bootS_file) $(boot_link_script)
	mkdir -p $(obj_dir) $(lst_dir) $(bin_dir)
	gcc -nostdlib -o $@ -T $(boot_link_script) $(src_bootS_file)
	gcc -T $(boot_link_script) -c -g -Wa,-a,-ad $(src_bootS_file) -o $(boot_obj_file) > $(boot_lst_file)
# the main files
$(main_bin_file):$(src_main_file) $(obj_func_file) $(src_sprintf_file) $(font_obj_file)
	mkdir -p $(obj_dir) $(lst_dir)	 $(bin_dir)
	gcc -nostdlib -m32 -c -o $(main_obj_file) $(src_main_file) -I .
	gcc -nostdlib -m32 -c -o $(sprintf_obj_file) $(src_sprintf_file)
	gcc -nostdlib -m32 -c -o $(head_obj_file) $(src_head_file)
	ld -m elf_i386 -o $@ -T $(main_link_script) -e $(bootpack_func) --oformat=binary $(main_obj_file) $(obj_func_file) $(font_obj_file)  $(head_obj_file) $(sprintf_obj_file) 

$(system_img_file): $(boot_bin_file) $(main_bin_file)
	cat $(boot_bin_file) $(main_bin_file) > $@
$(target_img_file):$(system_img_file) $(ipl_bin_file)
	mformat -f 1440 -B $(ipl_bin_file) -C -i $(target_img_file) ::
	mcopy $(system_img_file) -i $(target_img_file) ::


all:$(target_img_file)

run:$(target_img_file)
	$(QEMU) -m 32 -drive format=raw,file=$(target_img_file),if=floppy

debug:all
	$(QEMU) -drive format=raw,file=$(target_img_file),if=floppy -gdb tcp::10000 -S

clean:
	rm -rf $(target_dir)

