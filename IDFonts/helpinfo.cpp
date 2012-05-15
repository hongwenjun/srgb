#include "idtxt.h"

// 亮绿， 打印帮助信息
void helpinfo(const char* exefile)
{
    SetConsoleColor(0xA);
    printf("\n打包 Adobe文档中的字体工具, 支持格式Photoshop InDesign Illustrator等\n"
           "(C) 版权所有 2012.05 Hongwenjun (蘭公子)\n\n"
           "Usage: %s  <Adobe_Doc.indd>      [SaveFontPath]\n"
           "用法1: %s  <InDesign文档.indd>   [存放字体目录]\n"
           "用法2: %s  <Photoshop文档.psd>   [存放字体目录]\n"
           "用法3: %s  <Illustrator文档.ai>  [存放字体目录]\n" , exefile, exefile, exefile , exefile);
}
