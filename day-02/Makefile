src=hello-os.asm
src_pro=hello-pro.asm
bootstrap=boot.img
bootstrap_pro=boot-pro.img
boot:boot.img
boot-pro:boot-pro.img

boot.img:hello-os.asm
	nasm $(src) -o $(bootstrap)
boot-pro.img:hello-pro.asm
	nasm $(src_pro) -o $(bootstrap_pro)
fat12:
	@dd if=/dev/zero of=$(bootstrap) seek=1 count=2879 >> /dev/zero
	@ls -al $(bootstrap)
fat12-pro:
	@dd if=/dev/zero of=$(bootstrap_pro) seek=1 count=2879 >> /dev/zero
	@ls -al $(bootstrap_pro)
loop:fat12
	-sudo umount /mnt
	sudo mount -t msdos -o loop,fat=12   $(bootstrap) /mnt
run:boot fat12
	qemu-system-i386  -drive file=$(bootstrap),if=floppy
run-pro:boot-pro fat12-pro
	qemu-system-i386  -drive file=$(bootstrap_pro),if=floppy
clean:
	rm $(bootstrap)
	rm $(bootstrap_pro)



