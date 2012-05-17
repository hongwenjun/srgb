#include "idtxt.h"
#include "psdfonts.h"

// 子程序处理 Photoshop格式(*.psd) 模块，收集psd格式字体信息
int psdFonts(int argc, char* argv[])
{
    // 中文字体名和英文名对应表
    extern string FontsNameTable;

    const char* psdFileName = argv[1];
    const char* OutPath = ".\\Fonts";
    if (argc > 2)
        OutPath = argv[2];

    fstream psdfile(psdFileName, fstream::in | fstream::binary);
    size_t file_size = get_file_Size(psdfile); // 获得文件大小
    char* buffile = new char[file_size + 1];  // 文件读到缓冲
    psdfile.read(buffile, file_size);

    map<string, size_t> map_PSDFontName;
    map<string, size_t> map_zhCN_FontName; // 中文名字体
    map<string, size_t>::iterator iter_Fnt;
    // 搜索PSD文件缓存，把字体PsName装载到容器 map_PSDFontName
    if (SearchFontSet_PsnameToMap(buffile, file_size, map_PSDFontName))
        ;
    else
        cout << "好像 Phoshop 文档中没用使用字体，请确认文档是否存在!\n";

    delete[] buffile; // 释放文件缓冲;

    size_t pos, pos_beg;
    iter_Fnt = map_PSDFontName.begin();
    while (iter_Fnt != map_PSDFontName.end()) {
        pos = FontsNameTable.find(iter_Fnt->first);
        if (pos != string::npos) {
            pos_beg = FontsNameTable.rfind("<", pos) + 1;
            pos = FontsNameTable.find(">", pos_beg);
            string str(FontsNameTable, pos_beg , pos - pos_beg);
            ++map_zhCN_FontName[str];   // 使用 中文字体名
        } else
            ++map_zhCN_FontName[iter_Fnt->first]; // 否则使用 PS字体Name
        iter_Fnt++;
    }
    map_PSDFontName.swap(map_zhCN_FontName);  // 交换


    fstream coutlog("使用字体报告.txt", fstream::out | fstream::app);

    if (1) {
        cout << endl << psdFileName << "  文档使用字体的报告:" << endl ;
        coutlog << endl << "-------------------------------------------"
                << endl << psdFileName << " 字体报告:" << endl ;
        iter_Fnt = map_PSDFontName.begin();
        while (iter_Fnt != map_PSDFontName.end()) {
            cout << (iter_Fnt)->first << "\n";
            coutlog << (iter_Fnt++)->first << "\n";
        }
    }

    helpinfo(argv[0]);  //亮绿， 打印帮助信息
    return 10;
}
