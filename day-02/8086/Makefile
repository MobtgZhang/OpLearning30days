src=hello-os.asm

bootstrap=boot.img

boot:boot.img


boot.img:hello-os.asm
	nasm $(src) -o $(bootstrap)

fat12:
	@dd if=/dev/zero of=$(bootstrap) seek=1 count=2879 >> /dev/zero
	@ls -al $(bootstrap)
loop:fat12
	-sudo umount /mnt
	sudo mount -t msdos -o loop,fat=12   $(bootstrap) /mnt
run:boot fat12
	qemu-system-i386  -drive file=$(bootstrap),if=floppy
clean:
	rm $(bootstrap)



