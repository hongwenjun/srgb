/* hello.S: Hello world! from Loongson 2F */
#include <asm/unistd.h>
# define STDOUT 1
    .set mips3     # 默认MIPS2不支持dla，指定使用 MIPS III 
 
    .data
msg: .asciiz "Hello world!\n"

    .text
    .global main
    .ent main
main:

    li $a0, STDOUT  
    dla $a1, msg    # 龙芯2F_debian 地址好像是32位，使用 la 也可以  
    li $a2, 13
    li $v0, __NR_write
    syscall

exit:
    li $a0, 0
    li $v0, __NR_exit
    syscall
    .end main

/*
ssize_t write(int fd, const void *buf, size_t nbytes);
ssize_t read(int fd,void *buf,size_t count);

*/
