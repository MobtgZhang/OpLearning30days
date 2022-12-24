.arch i486
.text

.global io_hlt,io_cli,io_sti,io_stihlt
.global io_in8,io_in16,io_in32
.global io_out8,io_out16,io_out32
.global io_load_eflags,io_store_eflags

io_hlt:
    hlt
    ret
io_cli:
    cli
    ret
io_sti:
    sti
    ret
io_stihlt:
    sti
    hlt
    ret
io_in8:
    movl 4(%esp), %edx
    movl $0, %eax
    inb %dx, %al
    ret
io_in16:
    movl 4(%esp), %edx
    movl $0, %eax
    inw %dx, %ax
    ret
io_in32:
    movl 4(%esp), %edx
    inl %dx, %eax
    ret
io_out8:
    movl 4(%esp), %edx
    movb 8(%esp), %al
    outb %al, %dx
    ret
io_out16:
    movl 4(%esp), %edx
    movl 8(%esp), %eax
    outw %ax, %dx
    ret
io_out32:
    movl 4(%esp), %edx
    movl 8(%esp), %eax
    outl %eax, %dx
    ret
io_load_eflags:
    pushf
    pop %eax
    ret
io_store_eflags:
    movl 4(%esp), %eax
    push %eax
    popf
    ret
