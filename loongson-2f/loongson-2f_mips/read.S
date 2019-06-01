/* read.S: read from keyboard */
#include "util.S"
    .set mips3 

    .data
inMsg:  .space 255
prompt: .asciiz "What's you name?\n"
msg:    .asciiz "Glad to meet you! "

    .text
    .global main
    .ent main
main:
    print prompt, 18

    /* ssize_t read(int fd,void *buf,size_t count); */
    li $a0, STDIN
    dla $a1, inMsg
    li $a2, 255
    li $v0, __NR_read
    syscall

    print msg, 18
    print inMsg, 255

    exit 0
    .end main


