des_dir=bin
src_file=hello.S
obj_file=$(des_dir)/hello.o
elf_file=$(des_dir)/boot.elf
boot_file=$(des_dir)/boot.img
asm_file=$(des_dir)/boot.asm


$(boot_file):$(hello.S)
	mkdir -p $(des_dir)
	gcc -c $(src_file) -m32 -o $(obj_file)
	ld -m elf_i386 $(obj_file) -e start -Ttext 0x7c00 -o $(elf_file)
	objcopy	-S -O binary -j .text $(elf_file) $(boot_file)
	objdump -S $(elf_file) > $(asm_file)
fat12:
	@dd if=/dev/zero of=$(boot_file) seek=1 count=2879 >> /dev/zero
	@ls -al $(boot_file)

run:$(asm_file)
	qemu-system-i386 -fda $(boot_file)
clean:
	rm -rf $(des_dir)
