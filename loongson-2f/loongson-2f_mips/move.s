/* move.s  Move data between register  */
        .text
        .global main

        .ent main  
main:   li $t0, 8       # t0=8
        move $t1, $t0   # t1=t0
        move $t2, $t1   # t2=t1
        move $t3, $t2   # t3=t2

        move $v0, $t3   # v0=t3  返回值
        jr $ra
        .end main
