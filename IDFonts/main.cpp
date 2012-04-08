#include "idtxt.h"

int main(int argc, char* argv[])
{
    const char* idFileName = "test.indd";
    if (argc > 1)
        idFileName = argv[1];
    const char* OutPath = ".\\Fonts";
    if (argc > 2)
        OutPath = argv[2];
    CreateDirectory(OutPath, NULL);

    fstream idfile(idFileName, fstream::in | fstream::binary);
    size_t idf_size = get_file_Size(idfile); // 获得文件大小
    char* buffile = new char[idf_size + 1];  // 文件读到缓冲
    idfile.read(buffile, idf_size);

    const char* flag_beg = "<stFnt:fontFileName>";
    const char* flag_end = "</stFnt:fontFileName>";
    map<string, size_t> map_fontFileName;
    map<string, size_t>::iterator iter_Fnt;
    // 搜索文件缓存，把fontFileName装载到容器 map_fontFileName
    if (menSearch_FlagToMap(buffile, idf_size, flag_beg, flag_end, map_fontFileName))
        ;
    else
        cout << "InDesign文档.indd 做为参数，请确认文档是否存在!\n";

    map<string, string> map_SystemFonts;
    map<string, string>::iterator iter_SFt;
    // 加载系统字体表到字体容器
    if (LoadSystemFonts(map_SystemFonts))
        ;

    vector<string> vec_fontFullPath;
    vector<string>::iterator iter_Ffp;

    // 文件字体文件和系统字体文件交集
    iter_Fnt = map_fontFileName.begin();
    while (iter_Fnt != map_fontFileName.end()) {
        iter_SFt = map_SystemFonts.begin();
        while (iter_SFt != map_SystemFonts.end()) {
            if (iter_SFt->second.find(iter_Fnt->first) != string::npos) {
                vec_fontFullPath.push_back(iter_SFt->second);
                cout << iter_SFt->first << "\t\t" << iter_SFt->second << endl;
            }
            ++iter_SFt;
        }
        ++iter_Fnt;
    }

    // 获得系统字体目录的路径  windows\fonts
    char SysFontS_Path[MAX_PATH];
    char DstFontS_Path[MAX_PATH];
    GetWindowsDirectory(SysFontS_Path, MAX_PATH);
    strcat(SysFontS_Path, "\\Fonts\\");
    SetConsoleColor(0xE);  //亮黄
    // vec_fontFullPath 系统字体目录的字体修复全路径，复制字体到相应目录
    iter_Ffp = vec_fontFullPath.begin();
    while (iter_Ffp != vec_fontFullPath.end()) {
        if (iter_Ffp->find("\\") == string::npos)
            *iter_Ffp = string(SysFontS_Path) + *iter_Ffp;

        strcpy(DstFontS_Path, OutPath);
        strcat(DstFontS_Path, "\\");
        strcat(DstFontS_Path, GetFileBaseName(iter_Ffp->c_str()));
        cout << *iter_Ffp << "\n--------------------------------------------------------------------> " << "复制";
        CopyFile(iter_Ffp->c_str() , DstFontS_Path , TRUE);
        cout << "完成\n";
        iter_Ffp++;
    }

    SetConsoleColor(0xA);  //亮绿
    printf("\n打包 Adobe InDesign 文档中的字体工具\n"
           "(C) 版权所有 2012.01 Hongwenjun (蘭公子)\n\n"
           "Usage: %s  <InDesign_Doc.indd>  [SaveFontPath]\n"
           "用 法: %s  <InDesign文档.indd>  [存放字体目录]\n", argv[0] , argv[0]);

    // 没有找到的字体报告
    if (map_fontFileName.size() != vec_fontFullPath.size()) {
        stringstream oss;
        iter_Ffp = vec_fontFullPath.begin();
        while (iter_Ffp != vec_fontFullPath.end()) {
            oss << *iter_Ffp++ << endl;
        }
        string FoundFontsName(oss.str());
        SetConsoleColor(0xC);  //高亮红
        printf("\n********************************************************************************");
        iter_Fnt = map_fontFileName.begin();
        while (iter_Fnt != map_fontFileName.end()) {
            if (FoundFontsName.find(iter_Fnt->first) == string::npos)
                cout << iter_Fnt->first << "\a\t";
            ++iter_Fnt;
        }
        printf("\n\n以上字体没有找到文件，确认否安装到WINDOWS系统中\n"
               "如果在C:\\Program Files\\Common Files\\Adobe\\Fonts中,为Adobe软件共用字体,不用复制\n");
        SetConsoleColor(0x7);  //恢复
    }




    if (1) {
        flag_beg = "<stFnt:fontName>";
        flag_end = "</stFnt:fontName>";
        map<string, size_t> map_fontName;
// 搜索文件缓存，把fontName装载到容器 map_fontName
        menSearch_FlagToMap(buffile, idf_size, flag_beg, flag_end, map_fontName);

        cout << endl << idFileName << "  文档使用字体(英文名)的报告:" << endl ;
        iter_Fnt = map_fontName.begin();
        while (iter_Fnt != map_fontName.end())
            cout << (iter_Fnt++)->first << "\t";

    }

    delete[] buffile;
    return 0;
}

/***************************** 代码储备，不参与编译 ****************************/
#define CODE_STORE_NOT_COMPILE
#ifndef CODE_STORE_NOT_COMPILE




#endif // CODE_STORE_NOT_COMPILE


