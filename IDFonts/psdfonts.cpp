#include "idtxt.h"
#include "psdfonts.h"

// 子程序处理 Photoshop格式(*.psd) 模块，收集psd格式字体信息
int psdFonts(int argc, char* argv[])
{
    const char* psdFileName = argv[1];
    const char* OutPath = ".\\Fonts";
    if (argc > 2)
        OutPath = argv[2];

    fstream psdfile(psdFileName, fstream::in | fstream::binary);
    size_t file_size = get_file_Size(psdfile); // 获得文件大小
    char* buffile = new char[file_size + 1];  // 文件读到缓冲
    psdfile.read(buffile, file_size);

    const char* flag_beg = "/FontSet [";
    const char* flag_end = "]";

    size_t  count = file_size;
    const char* buffer = buffile;

    const char* ps = buffer;
    char* pch = NULL;
    char* pch2 = NULL;
    pch = memfind(ps, flag_beg , count);
    while (pch != NULL) {
        pch2 = memfind(pch, flag_end , 1024);
        if (pch2 != NULL) {
            string str(pch + strlen(flag_beg) , pch2);
            {
                // Utf-8 转 GBK string版本，按需求是否屏蔽改语句
                // ConvertUtf8ToGBK(str);
            }
            cout << str ;
            // ++flag_map[str]; // 装载到字符串容器
        }
        ps = pch + strlen(flag_beg);
        pch = memfind(ps, flag_beg , count - (ps - buffer));
    }


    delete[] buffile; // 释放文件缓冲;

    helpinfo(argv[0]);  //亮绿， 打印帮助信息
    return 10;
}
