/* add.s  汇编 78 + 87 = ?  */
        .text
        .global main

        .ent main  # 
main:   li $t0, 78
        li $t1, 87
        add $v0, $t0, $t1     # v0 = t0 + t1
        jr $ra
        .end main
