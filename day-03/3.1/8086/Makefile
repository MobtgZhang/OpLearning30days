src_file=disk.asm
disk_read_img_file=disk.img

disk:disk.img

disk.img:disk.asm
	nasm $(src_file) -o $(disk_read_img_file)
fat12:
	@dd if=/dev/zero of=$(disk_read_img_file) seek=1 count=2879 >> /dev/zero
	@ls -al $(disk_read_img_file)
loop:fat12
	-sudo umount /mnt
	sudo mount -t msdos -o loop,fat=12   $(disk_read_img_file) /mnt
run:disk fat12
	qemu-system-i386  -drive file=$(disk_read_img_file),if=floppy
clean:
	rm $(disk_read_img_file)
