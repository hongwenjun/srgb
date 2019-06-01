/* var2.s : with our own dla */ 
 
    .set mips3     # 默认MIPS2不支持dla，指定使用 MIPS III 

    .macro dla reg, label
    lui \reg, %highest(\label)        # Error: relocation %highest isn't supported by the current ABI 
                                      # 当前ABI不支持重定位 %highest
    daddiu \reg, \reg, %highest(\label)
    dsll \reg, \reg, 16
    daddiu \reg, \reg, %hi(\label)
    dsll \reg, \reg, 16
    daddiu \reg, \reg, %lo(\label)
    .endm

    .data
    .align 4
var: .word 3


    .text
    .global main
    .ent main
main:
    li $t0, 8       # t0 = 8
    dla $t1, var    # t1 = address of var
    sw $t0, 0($t1)  # var = 8

    lw $v0, 0($t1)  # v0 = var
    jr $ra
    .end main
