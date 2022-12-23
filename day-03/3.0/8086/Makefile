src_file=hello.nas
bootstrap=boot.img

boot:boot.img

$(bootstrap):$(src_file)
	nasm $(src_file) -o $(bootstrap)

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