src_file=head.S
obj_file=head.o
elf_file=head.elf
asm_file=head.asm
boot_file=boot.img
kernel_ld_file=kernel.ld
kernel_obj_files=main.o screen.o
address=0xc400
CFLAGS=-I.  -fno-stack-protector

%.o:%.c
	gcc	-nostdinc $(CFLAGS) -c $< -m32 -o $@
%.o:%.S
	gcc -nostdinc $(CFLAGS) -c $< -m32 -o $@

all:head main
	cat main.out >> head.out
	mv head.out kernel.out
head:head.o
main:$(kernel_obj_files)

head:
	ld -m elf_i386 $(obj_file) -e start -Ttext $(address) -o $(elf_file)
	objcopy	-S -O binary -j .text $(elf_file) $(boot_file)
	objdump -S $(elf_file)>$(asm_file)
main:
	@echo "addr=0x00"
	ld -m elf_i386 -e bootmain -T $(kernel_ld_file) -o main.elf $(kernel_obj_files)
	objdump -S -D $@.elf >$@.asm
	objcopy -S -O binary $@.elf $@.out
fat12:
	@dd if=/dev/zero of=$(boot_file) seek=2 count=2878 >> /dev/zero
	@ls -al $(boot_file)
run:
	qemu-system-i386 -drive file=$(boot_file),if=floppy
clean:
	-rm -f $(boot_file) *.elf *.out *.asm *.o
