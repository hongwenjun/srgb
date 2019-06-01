/* This is MIPS64 program For 龙芯2F */
    .text
    .global main

    .ent main   # 函数 main
main:  
    nop
    LI $v0, -88   # v0: 返回值
    abs $v0, $v0
    JR $ra      # 返回
    .end main   # 退出 main

