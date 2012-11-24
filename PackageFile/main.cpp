 #include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <cstdio>
#include <cctype>
#include <regex>
#include <windows.h>

using namespace std;

// 简单的文件属性结构
struct F_STRUCT {
    char date[16];
    char time[16];
    char size[255];
    char name[255];
};

// 全局变量
char ConfigFile[MAX_PATH] = {0};
char AppPath[MAX_PATH] = {0};
string Path_Argv;
string Reg_Argv;

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
PATH  =   D:\我的照片\测试目录
  ;  PATH  =   D:\我的照片\测试目录

REGEX =     2005-\d\d-\d\d
 ;  REGEX =     2005-\d\d-\d\d

***************************************************************************/



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

/** TODO (hongwenjun#1#): 现在不能处理文件名重名问题
实现了读取配置文件INI，识别注释语句
*/

int main(int argc , char* argv[])
{

    GetAppDir(ConfigFile);
    strcat(ConfigFile, "\\PACKAGE_FILE.ini");
    if (IsFileExist(ConfigFile))
        LoadConfigFile();

    printf("%s\n", ConfigFile);
    // 输入文件   D:\\名片\\date.txt   D:\\名片\\file.txt
    // DIR . /S /AA /TW >\date.txt  和  DIR . /S /AA /B >\file.txt
    ifstream datefile("\\date.txt ");
    ifstream pathfile("\\file.txt ");
    ofstream listfile("\\listfile.txt");

    for (int i = 1 ; i != argc; ++i) {
        const char* parg = argv[i];
        if (*parg == '-') {
            switch (* (parg + 1)) {
            case 'p':
                Path_Argv = string(parg + 2);  // 扫描路径参数
                break;

            case 'e':
                Reg_Argv = string(parg + 2);  ;  // 正则参数
                break;
            default:
                break;
            }
        }
    }
    printf("%s\n%s\n", Path_Argv.data() , Reg_Argv.data()) ; // app.exe -pD:\CodeBlocks -e\w+\.dll

    string readline;    // 读取每行，然后使用正则搜索匹配
    smatch m;
    regex e(Reg_Argv);
    regex re("(\\w\\:\\\\)(.*\\\\([^\\\\]+))");  // 公式(\w\:\\)(.*\\([^\\]+)) 匹配 D:\名片\89届初中毕业生通讯录.cdr



    F_STRUCT d_file ;  // 简单的文件属性结构

    map<string, string> mss_date;
    map<string, string> mss_path;

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
        // cout << mss_path[it->second] << endl;
        listfile << mss_path[it->second] << endl;  // 输出结果
    }

    datefile.close();  pathfile.close();   listfile.close();
    system("pause");
//    system("7z a -ttar -r -scsWIN \\backup.tar @\\listfile.txt"); // 调用7z命令行打包文件
    return 0;

}

