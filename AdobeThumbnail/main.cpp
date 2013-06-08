#include "atpch.h"

using namespace std;

void helpinfo(const char* exefile)
{
    printf("\nAdobe AI EPS INDD 缩略图转成 JPEG图片 检测文档版本工具\n"
           "(C) 版权所有 2013.06 Hongwenjun (蘭公子)\n\n"
           "Usage: %s  <Adobe文件.ai>  [SaveJpgFile]\n" , exefile);
}

int main(int argc , char* argv[])
{

    const char* adobe_filename = "AICS5.ai";
    const char* savejpeg_filename = "AI.Thumb.jpg";

    if (argc == 2)
        adobe_filename = argv [1];
    if (argc == 3)
        savejpeg_filename = argv [2];

    int ret = AdobeThumbnail(adobe_filename , savejpeg_filename);   // 执行


    helpinfo(argv[0]);

    printf("请看 %s ----> %s\n" , adobe_filename , savejpeg_filename);

    return ret;
}
