 /*
** Copyright (C) Http://sRGB.GoogleCode.com All rights reserved.
*/
#include "pkfile.h"



// 全局变量
char ConfigFile[MAX_PATH] = {0};
string Path_Argv;
string Reg_Argv;
string Packfile_Argv;
bool  Direct_Datelog_Flag = false;
bool  Delete_Datelog_Flag = false;
bool  Test_List_Flag = false;


// 删除字符串前后空白
string strTrim(string& str)
{
    str = str.substr(str.find_first_not_of("\t \n"));
    str = str.substr(0, str.find_last_not_of("\t \n") + 1);
    return str;
}

// 删除字符串前后空白
char* csTrim(char* cs)
{
    char* ret = cs;
    char* pc = new char[strlen(cs) + 1];

    int pos = strspn(cs , "\t \n");     // 查找非空白处pos
    strcpy(pc , cs + pos);

    char* end = pc;
    while (*end++)     // 找到字符串末位的'\0'，
        ;              // 因为上面 end++，实际指针在 '\0'的后一个
    end -= 2;          // 所以退2格，回到字符串最后一个字母

    while (*end == ' ' || *end == '\t' ||  *end == '\n') {
        *end-- = '\0';
    }
    strcpy(cs , pc);

    delete[] pc;
    return ret;
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

        } else if (line.find("DIRECTDATE") != string::npos) {
            line = line.substr(line.find('=') + 1);
            // 如果有date.txt ,不用每次扫描目录,如果更新了,手工删除date.txt
            char df_path[MAX_PATH] = {0}; GetAppDir(df_path); strcat(df_path , "\\date.txt");
            Direct_Datelog_Flag = (strTrim(line) == "1") && (IsFileExist(df_path));
        }
    }
    file.close();
}

/*****************   PACKAGE_FILE.ini  文件示例   *************************
;PkFile 1.00  Copyright (c) Hongwenjun(蘭公子)  2012-11-28
;
;Usage: PkFile.exe  -p{打包路径}  -e{正则公式}
;
;示 例：PkFile.exe   -pD:\我的照片 -e2012-11
;       可以打包指定目录2012年11月建立或修改的文件

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
           "Usage: PkFile.exe  -p{打包路径}   -o{输出文件}   -e{正则公式}  [-d -l] \n"
           "       使用 -d 命令行开关，重新扫描目录，完成后删除扫描记录\n\n"
           "示 例: PkFile.exe  -pD:\\我的照片  -oD:\\Temp\\backup.tar  -e2012-11\n"
           "       可以打包指定目录2012年11月建立或修改的文件\n\n"
           "直接修改配置文件 PACKAGE_FILE.ini 中的PATH和REGEX 参数，等价控制台输入参数\n"
           "PACKFILE 和 DIRECTDATE 等详细见示例文件 PACKAGE_FILE.ini \n\n");
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
            case 'p': case 'P':
                Path_Argv = string(parg + 2);  // 扫描路径参数
                break;
            case 'e': case 'E':
                Reg_Argv = string(parg + 2);   // 正则参数
                break;
            case 'o': case 'O':
                Packfile_Argv = string(parg + 2);   // 输出文件参数
                break;

            case 'd': case 'D':
                Direct_Datelog_Flag = false;  // 重新扫描目录
                Delete_Datelog_Flag = true;   // 完成后删除扫描记录 date.txt
                break;
            case 'l': case 'L':
                Test_List_Flag = true;   // 只是显示 listfiel.txt, 不打包文件
                break;
            case 'h': case 'H':
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



FILE* pf_path_file;
// 搜索lp_path目录包括子目录所有文件名，保存到输出out_file_name
void find_path_save_file(const char* lp_path , const char* out_file_name)
{
    pf_path_file = fopen(out_file_name, "w");
    find_path(lp_path);
    fclose(pf_path_file);
}

// 遍历搜索目录
void find_path(const char* lp_path)
{
    HANDLE hFile;  // windows对文件的操作首先要得到一个文件句柄
    FILETIME ft;  SYSTEMTIME st; // 文件时间 和 系统时间
    WIN32_FIND_DATAA wfd;  // WIN32_FIND_DATAA 是windows定义的查找文件的结构
    char sz_path[1024] = {0};
    char buf[1024] = {0};
    strcpy(sz_path, lp_path);
    strcat(sz_path, "\\*.*");
    hFile = FindFirstFile(sz_path, &wfd);  // FindFirstFile函数查找一个文件，sz_path是要查找的文件名，可以是全路径或相对路径，也可以写通配符，如“c:/*.*”

    if (hFile != INVALID_HANDLE_VALUE) {
        while (FindNextFile(hFile, &wfd)) {  // 利用第一次找到的文件句柄，继续寻找下个文件，如果找到下个文件，则函数填充wfd结构，并返回true
            if (wfd.cFileName[0] == '.')continue; // . 当前目录 .. 上级目录符号  跳过去

            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { // 判断，得出找到的文件名是否是一个文件夹，如果是，则递归调用查找函数，由此实现深度全文件夹搜索
                strcpy(sz_path, lp_path);
                strcat(sz_path, "\\");
                strcat(sz_path, wfd.cFileName);
                find_path(sz_path);   // 调用递归查找

            } else {
                ft = wfd.ftLastWriteTime;
                FileTimeToSystemTime(&ft, &st);

                sprintf(buf , "%d-%02d-%02d  %02d:%02d  %10u | ", st.wYear, st.wMonth, st.wDay, st.wHour + 8, st.wMinute , wfd.nFileSizeLow);
                sprintf(buf + 32, "%s | %s", wfd.cFileName , lp_path);

                fprintf(pf_path_file , "%s\n", buf);
            }
        }
    }

}

