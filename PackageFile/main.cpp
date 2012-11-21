 #include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <cstdio>
#include <cctype>
#include <regex>

struct F_STRUCT {   // 简单的文件属性结构
    char date[16];
    char time[16];
    char size[255];
    char name[255];
};

using namespace std;

int main(int argc , char* argv[])
{

    // 输入文件   D:\\名片\\date.txt   D:\\名片\\file.txt
    // DIR . /S /AA /TW >\date.txt  和  DIR . /S /AA /B >\file.txt
    ifstream datefile("/date.txt ");
    ifstream pathfile("/file.txt ");
    ofstream listfile("/listfile.txt");

    string path_argv = "D:\\名片";
    string reg_argv  = "2010-\\d\\d-\\d\\d" ;// 2010-06-30  16:51            20,780 89届初中毕业生通讯录.cdr
    for (int i = 1 ; i != argc; ++i) {
        const char* parg = argv[i];
        if (*parg == '-') {
            switch (* (parg + 1)) {
            case 'p':
                path_argv = string(parg + 2);  // 扫描路径参数
                break;

            case 'e':
                reg_argv = string(parg + 2);  ;  // 正则参数
                break;
            default:
                break;
            }
        }
    }
    cout << path_argv << reg_argv ; // app.exe -pD:\CodeBlocks -e\w+\.dll

    string readline;    // 读取每行，然后使用正则搜索匹配
    smatch m;
    regex e(reg_argv);
    regex re("(\\w\\:\\\\)(.*\\\\([^\\\\]+))");  // 公式(\w\:\\)(.*\\([^\\]+)) 匹配 D:\名片\89届初中毕业生通讯录.cdr

    /** TODO (hongwenjun#1#): 现在不能处理文件名重名问题 */

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
        cout << mss_path[it->second] << endl;
        listfile << mss_path[it->second] << endl;  // 输出结果
    }

    datefile.close();  pathfile.close();   listfile.close();

//    system("7z a -ttar -r -scsWIN \\backup.tar @\\listfile.txt"); // 调用7z命令行打包文件
    return 0;
}

