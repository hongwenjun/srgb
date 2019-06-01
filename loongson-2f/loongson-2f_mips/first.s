/* This is MIPS64 program For 龙芯2F */
        .text
        .global main

        .ent main   # 函数 main
main:   nop
        LI $v0, 8   # v0: 返回值
        JR $ra      # 返回
        .end main   # 退出 main

