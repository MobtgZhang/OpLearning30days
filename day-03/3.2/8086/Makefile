src_vga_file=vga.asm
src_vga_old_file=vga-old.asm
boot_img_file=vga.img
boot_img_old_file=vag-old.img

default:
	nasm $(src_vga_file) -o $(boot_img_file)
run:
	nasm $(src_vga_file) -o $(boot_img_file)
	qemu-system-i386 -drive file=$(boot_img_file),if=floppy
clean:
	rm -f $(boot_img_file) $(boot_img_old_file)
old:
	nasm $(src_vga_old_file) -o $(boot_img_old_file)
	qemu-system-i386 -drive file=$(boot_img_old_file),if=floppy
