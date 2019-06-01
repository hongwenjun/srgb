/* var.s
 * simple variable
 */

    .data
var: .word 3

    .set mips3     # 默认MIPS2不支持dla，指定使用 MIPS III 

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
