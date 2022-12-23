void boot_main(void){
finish:
    __asm__("hlt\n\t");
    goto finish;
}
