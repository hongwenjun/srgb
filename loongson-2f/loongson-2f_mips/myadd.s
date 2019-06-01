/* myadd.s:  macro defination and usage */
    .macro myadd reg, a, b
    li \reg, \a
    addi \reg, \reg, \b
    .endm

    .text
    .global main
    .ent main
main:
    myadd $t0 , 3 , 5
    move  $v0, $t0
    jr $ra
    .end main

