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
    // DIR . /S /AA /TW >date.txt  和  DIR . /S /AA /B >file.txt
    ifstream datefile("D:\\名片\\date.txt ");

    string readline;    // 读取每行，然后使用正则搜索匹配
    smatch m;
    regex e("20\\d\\d-\\d\\d-\\d\\d");     // 2010-06-30  16:51            20,780 89届初中毕业生通讯录.cdr

    F_STRUCT d_file ;  // 简单的文件属性结构

    map<string, string> mss;

    while (getline(datefile , readline)) {

        if (regex_search(readline, m, e)) {      // 符合条件的的文件列表存如容器
            sscanf(readline.c_str(), "%s %s %s" , d_file.date, d_file.time , d_file.size);
            sprintf(d_file.name , "%s" , readline.c_str() + 36);    // 为了处理文件名有空格，所以只好分两行读

            //   cout << d_file.name  << " -> " << d_file.date << endl;
            mss.insert(make_pair(string(d_file.date) + " -> " + d_file.name  , string(d_file.name)));
        }

    }

    for (auto it = mss.begin() ; it != mss.end(); ++it)
        cout << it->first << endl;

    return 0;
}
