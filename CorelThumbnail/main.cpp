/*
** Copyright (C) http://srgb.googlecode.com  All rights reserved.
*/
#include <iostream>
#include "corel_thumbnail.h"



void helpinfo(const char* exefile)
{
    printf("\nCorelDRAW 缩略图转成PNG图片 cdr文档版本信息 V1.0\n"
           "(C) 版权所有 2013.06 Hongwenjun (蘭公子)  http://srgb.googlecode.com \n\n"
           "Usage: %s  <CorelDRAW文件.cdr>  [SavePngFile]\n" , exefile);
}


// 读RIFF_CDR_TYPE 文件头和软件版本号
RIFF_CDR_TYPE cdr_riff ;

int main(int argc, char* argv[])
{


    const char* cdr_filename = "test.cdr";
    const char* png_filename = NULL;

    if (argc > 1)
        cdr_filename = argv[1];
    if (argc > 2)
        png_filename = argv[2];

    bool ret =  cdr_thumbnail_png(cdr_filename, png_filename);


    if (!ret) {
        helpinfo(argv[0]);
        return 88;

    }

    printf("CorelDRAW文件:  %s\n文件版本 Ver %d\n文件大小 %d 字节\n ", argv[1] , cdr_riff.version /100 , cdr_riff.cb+8);

    return 0;
}
