src_file=hello.S
obj_file=hello.o
elf_file=boot.elf
boot_file=boot.img
asm_file=boot.asm

$(boot_file):$(hello.S)
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
	-rm -f $(obj_file) $(elf_file) $(asm_file)
