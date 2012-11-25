/*
** Copyright (C) Http://sRGB.GoogleCode.com All rights reserved.
*/
#include "pkfile.h"



// 全局变量
char ConfigFile[MAX_PATH] = {0};
string Path_Argv;
string Reg_Argv;
string Packfile_Argv;


// 删除字符串前后空白
string strTrim(string& str)
{
    str = str.substr(str.find_first_not_of("\t \n"));
    str = str.substr(0, str.find_last_not_of("\t \n") + 1);
    return str;
}

// 读取和保存配置
void LoadConfigFile()
{
    string line;
    fstream file(ConfigFile , fstream::in);

    while (getline(file , line)) {

        // 跳过没有'='号 和 有';' 的注解行
        if (line.find('=') == string::npos)
            continue ;
        line = strTrim(line);
        if (line[0] == ';')
            continue ;

        // 获得路径和正则公式( 截取 = 符号之后，删除空白)
        if (line.find("PATH") != string::npos) {
            line = line.substr(line.find('=') + 1);
            Path_Argv = strTrim(line);

        } else if (line.find("REGEX") != string::npos) {
            line = line.substr(line.find('=') + 1);
            Reg_Argv = strTrim(line);

        } else if (line.find("PACKFILE") != string::npos) {
            line = line.substr(line.find('=') + 1);
            Packfile_Argv = strTrim(line);
        }
    }
    file.close();
}

/*****************   PACKAGE_FILE.ini  文件示例   *************************
;PkFile 1.00  Copyright (c) Hongwenjun(蘭公子)  2012-11-28
;
;Usage: PkFile.exe	-p{打包路径}  -e{正则公式}
;
;示 例：PkFile.exe	-pD:\我的照片 -e2012-11
; 	    可以打包指定目录2012年11月建立或修改的文件

;直接修改配置文件 = PACKAGE_FILE.ini 中的PATH和REGEX 参数，等价控制台输入参数
[PACKAGE_FILE]
; PATH  =   D:\我的照片\测试目录

PATH  =   D:\名片
REGEX =   2008-11-
; REGEX =     2005-\d\d-\d\d
;


; 备份文件名, 扩展名指定打包格式 tar不压缩速度快,  zip适中, 7z 压缩比高速度慢
PACKFILE = D:\Temp\backup.tar

; 可以添加参数设置密码和分包大小
; PACKFILE = D:\Temp\backup.zip -p123456 -v512m

***************************************************************************/

// 打印帮助信息
void print_help()
{
    printf("%s\n",
           "PkFile 1.00  Copyright (c) Hongwenjun(蘭公子)  2012-11-28\n\n"
           "Usage: PkFile.exe  -p{打包路径}   -o{输出文件}   -e{正则公式}\n\n"
           "示 例: PkFile.exe  -pD:\\我的照片  -oD:\\Temp\\backup.tar  -e2012-11\n"
           "       可以打包指定目录2012年11月建立或修改的文件\n\n"
           "直接修改配置文件 PACKAGE_FILE.ini 中的PATH和REGEX 参数，等价控制台输入参数\n\n");
}

// 参数初始化
int initial_arg(int argc , char* argv[])
{

    GetAppDir(ConfigFile);
    strcat(ConfigFile, "\\PACKAGE_FILE.ini");
    if (IsFileExist(ConfigFile))
        LoadConfigFile();

    for (int i = 1 ; i != argc; ++i) {
        const char* parg = argv[i];
        if (*parg == '-') {
            switch (* (parg + 1)) {
            case 'p':
                Path_Argv = string(parg + 2);  // 扫描路径参数
                break;
            case 'e':
                Reg_Argv = string(parg + 2);   // 正则参数
                break;
            case 'o':
                Packfile_Argv = string(parg + 2);   // 输出文件参数
                break;

            case 'h':
                print_help();  // 调用帮助
                return 88;
                break;
            default:
                break;
            }
        }
    }
//   printf("%s\n%s\n%s\n", ConfigFile, Path_Argv.data() , Reg_Argv.data()) ;
    return 0 ;
}


// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName)
{
    WIN32_FIND_DATA fd = {0};
    HANDLE hFind = FindFirstFile(lpFileName, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    }
    return ((hFind != INVALID_HANDLE_VALUE) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

// 功能 获得当前路径
char* GetAppDir(char* szPath)
{
    char* ret = szPath;
    GetModuleFileName(NULL, szPath, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    *(strrchr(szPath , '\\')) = '\0';   // 删除文件名，只留下目录
    return ret;
}
