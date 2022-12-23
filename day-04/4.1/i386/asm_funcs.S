.arch i486
.text

.global io_hlt
.global write_mem8

io_hlt:
    hlt
    ret

write_mem8:
    movl        4(%esp),%ecx
    movl        8(%esp),%al
    mov         %al,(%ecx)
    ret
