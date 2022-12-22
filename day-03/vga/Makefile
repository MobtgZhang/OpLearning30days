src_vga_file=vga.asm
boot_img_file=vga_test.img
default:
	nasm $(src_vga_file) -o $(boot_img_file)
run:
	nasm $(src_vga_file) -o $(boot_img_file)
	qemu-system-i386 -drive file=$(boot_img_file),if=floppy
clean:
	rm $(boot_img_file)
