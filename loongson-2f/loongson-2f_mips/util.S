/* util.S : utility macros */
#include <asm/unistd.h>
#define STDIN 0
#define STDOUT 1

# Loongson 2F, 默认MIPS2不支持dla，指定使用 MIPS III
    .set mips3   

/* exit macro */
    .macro exit n
    li $a0, 0
    li $v0, __NR_exit
    syscall
    .endm

/* print macro */
    .macro print msg, n
    li $a0, STDOUT
    dla $a1, \msg
    li $a2, \n
    li $v0, __NR_write
    syscall
    .endm

