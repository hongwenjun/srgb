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
    string file_txt = string(AppPath) + "\\file.txt";
    string listfile_txt = string(AppPath) + "\\listfile.txt";
    string pkcmd;

    if (_chdir(Path_Argv.c_str())) {
        printf("无法找到的目录: %s\n\a\n\n", Path_Argv.c_str());
        print_help();
        if (argc == 1) cin.get();
        return -1;
    } else if (!Direct_Datelog_Flag) {
        printf("正在扫描目录: %s\n", Path_Argv.c_str());
        // DIR . /S /AA /TW >\date.txt  和  DIR . /S /AA /B >\file.txt   调用系统获得数据输入文件
        pkcmd = string("DIR . /S /AA /TW > ") + date_txt;
        system(pkcmd.c_str());
        pkcmd = string("DIR . /S /AA /B > ") + file_txt;
        system(pkcmd.c_str());
    }

    ifstream datefile(date_txt.c_str());
    ifstream pathfile(file_txt.c_str());
    ofstream listfile(listfile_txt.c_str());

    string readline;    // 读取每行，然后使用正则搜索匹配
    smatch m;
    regex e(Reg_Argv);
    regex re("(\\w\\:\\\\)(.*\\\\([^\\\\]+))");  // 公式(\w\:\\)(.*\\([^\\]+)) 匹配 D:\名片\89届初中毕业生通讯录.cdr

    F_STRUCT d_file ;  // 简单的文件属性结构

    map<string, string> mss_date;
    map<string, string> mss_path;
    map<string, string> mss_list;

    while (getline(datefile , readline)) {

        if (regex_search(readline, m, e)) {      // 符合条件的的文件列表存如容器
            sscanf(readline.c_str(), "%s %s %s" , d_file.date, d_file.time , d_file.size);
            sprintf(d_file.name , "%s" , readline.c_str() + 36);    // 为了处理文件名有空格，所以只好分两行读

            //   cout << d_file.name  << " -> " << d_file.date << endl;
            mss_date.insert(make_pair(string(d_file.date) + " -> " + d_file.name  , string(d_file.name)));
        }
    }

    while (getline(pathfile , readline)) {
        if (regex_match(readline, m, re)) {     // regex_match 和 regex_search 这里都可以用
            //    cout <<  m[3] << " -> " <<  m[2] << endl;
            mss_path.insert(make_pair(m[3], m[2]));  // 文件名 和 路径装载到容器
        }
    }
    for (auto it = mss_date.begin() ; it != mss_date.end(); ++it) {
        //   cout << << "\t" << mss_path[it->second] << endl;
        mss_list.insert(make_pair(mss_path[it->second] , it->second));  // 结果存到容器，删除重复，目前还无法处理同名文件
    }
    for (auto it = mss_list.begin() ; it != mss_list.end(); ++it)
        listfile << it->first << endl;  // 输出结果

    datefile.close();  pathfile.close();   listfile.close();

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
        remove(date_txt.c_str()); remove(file_txt.c_str()); remove(listfile_txt.c_str());
    }
    if (argc == 1) cin.get();
    return 0;

}

