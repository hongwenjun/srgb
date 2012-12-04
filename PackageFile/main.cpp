/*
** Copyright (C) Http://sRGB.GoogleCode.com All rights reserved.
*/

#include "pk_pch.h"
#include "pkfile.h"
#include <direct.h>

/** TODO (hongwenjun#1#): 现在不能处理文件名重名问题
实现了读取配置文件INI，识别注释语句
*/

// 使用全局变量
extern string Path_Argv;
extern string Reg_Argv;
extern string Packfile_Argv;
extern bool  Direct_Datelog_Flag;
extern bool  Delete_Datelog_Flag;
extern bool  Test_List_Flag ;


int main(int argc , char* argv[])
{
    if (88 == initial_arg(argc , argv))
        return 88;

    char AppPath[MAX_PATH] = {0};
    GetAppDir(AppPath);

    string date_txt = string(AppPath) + "\\date.txt";
    string listfile_txt = string(AppPath) + "\\listfile.txt";
    string pkcmd;

    if (_chdir(Path_Argv.c_str())) {
        printf("无法找到的目录: %s\n\a\n\n", Path_Argv.c_str());
        print_help();
        if (argc == 1) cin.get();
        return -1;
    } else if (!Direct_Datelog_Flag) {
        printf("正在扫描目录: %s\n", Path_Argv.c_str());
        // 获得文件名和路径数据输入文件 ，和差不多DIR . /S /AA /TW >\date.txt
        find_path_save_file(Path_Argv.c_str(), date_txt.c_str());
    }

    ifstream datefile(date_txt.c_str());
    ofstream listfile(listfile_txt.c_str());

    string readline;    // 读取每行，然后使用正则搜索匹配
    smatch m;
    regex e(Reg_Argv);


    F_STRUCT d_file ;  // 简单的文件属性结构

    map<string, string> mss_date;

    while (getline(datefile , readline)) {
        char buf[256];
        char* pch = NULL;
        if (regex_search(readline, m, e)) {      // 符合条件的的文件列表存如容器
            //  sscanf(readline.c_str() + 32 , "%s | %s" ,  d_file.name, d_file.path);  // 不能处理路径和文件名中有空格
            sprintf(buf, "%s", readline.c_str() + 32);
            pch = strchr(buf, '|'); // 查找分割标记
            *pch = '\0';
            strcpy(d_file.name , buf);
            strcpy(d_file.path , pch + 1);
            csTrim(d_file.name);  csTrim(d_file.path); // 删除前后空格
            // cout << d_file.path << "\\" <<d_file.name << endl;
            mss_date.insert(make_pair(string(d_file.path) + "\\" + d_file.name  , string(d_file.name)));
        }
    }

    for (auto it = mss_date.begin() ; it != mss_date.end(); ++it)
        listfile << it->first.substr(it->first.find(":\\") + 2) << endl; // 输出结果

    datefile.close();  listfile.close();

    // 调用7z命令行打包文件，
    pkcmd = string("7z.exe a -scsWIN  ") + Packfile_Argv + " @" + listfile_txt;

    if (IsFileExist((string(AppPath) + "\\res\\7z.exe").c_str()))
        pkcmd = string(AppPath) + "\\res\\" + pkcmd;
    if (Test_List_Flag) pkcmd = string("TYPE ") + listfile_txt;   // 只是显示 listfiel.txt, 不打包文件

    _chdir("\\"); system("COLOR F9");
    if (system(pkcmd.c_str()) != 0) {
        cout << "调用打包命令:\t" << pkcmd << endl
             << "失败！请检查软件目录下是否有 7z.exe \n";
    }
    print_help();
    if (Delete_Datelog_Flag) {
        remove(date_txt.c_str());   remove(listfile_txt.c_str());
    }
    if (argc == 1) cin.get();
    return 0;

}

