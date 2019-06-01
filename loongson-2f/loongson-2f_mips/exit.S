/* exit.S exit system call */
#include <asm/unistd.h>


#    .set mips3     # 默认MIPS2不支持dla，指定使用 MIPS III 
 
    .text
    .global main
    .ent main
main:
    li $v0, __NR_exit   # system call number --> v0
    li $a0, 8           # fiirst param --> a0, status = 8
    syscall
    .end main
