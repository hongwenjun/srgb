#define PCRE_STATIC // 静态库编译选项

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcre.h"
#define OVECCOUNT 30 /* should be a multiple of 3 */
using namespace std;

std::string& regex_get_group(const std::string& src , const std::string& pattern ,
                             int group , std::string& str_group);

std::string& load_string(std::string& str, fstream& infile)  // 加载文件到string
{
    std::stringstream ss;
    ss << infile.rdbuf();
    str = ss.str();
    return str;
}

//  g++ -std=c++0x -lpcre -o getwanip   编译选项

int main()
{
    system("touch ./getwanip.txt");
    system("curl -s 192.168.1.1 | grep ipinfo >>./getwanip.txt");
    system("curl -s http://www.ip138.com/ips1388.asp | grep ip_add.asp  >>./getwanip.txt");
    system("curl -s http://ns1.dnspod.net:6666    >>./getwanip.txt");
    system("echo \"  \"  >>./getwanip.txt");
    system("curl -s http://myip.dnsdynamic.org/  >>./getwanip.txt");

    std::fstream fs("./getwanip.txt", std::fstream::in);
    std::string  src;  // 收集的网络 wanip数据
    std::string  reg  = "(\\d+\\.\\d+\\.\\d+\\.\\d+)";     // 简单匹配IP地址正则公式
    load_string(src, fs);
    fs.close();
    remove("./getwanip.txt");

    std::string ipaddr;
    regex_get_group(src, reg , 1 , ipaddr);
    if (ipaddr == "")
        return -1;

    std::map<string, int> map_wanip;
    while (ipaddr != "") {
              cout << ipaddr << endl;;
        ++map_wanip[ipaddr];
        src = src.substr(src.find(ipaddr) + ipaddr.size());
        regex_get_group(src, reg , 1 , ipaddr);
    }

    std::map< int , string> re_wanip;  // 按计数器重排IP地址MAP容器
    for (auto it = map_wanip.begin(); it != map_wanip.end() ; ++it) {
        re_wanip.insert(make_pair(it->second , it->first));
    }

    auto it = re_wanip.begin(); // 最大值排最后
    cout << it->second;
    return 0;
}

std::string& regex_get_group(const std::string& src , const std::string& pattern ,
                             int group , std::string& str_group)
{
    pcre*  re;
    const char* error;
    int  erroffset;
    int  ovector[OVECCOUNT];  // 用来返回匹配位置偏移量的数组

//    printf("原字符串 : %s\n", src.c_str());
//    printf("匹配公式 : \"%s\"\n", pattern.c_str());

    re = pcre_compile(pattern.c_str(),   // pattern, 输入参数，将要被编译的字符串形式的正则表达式
                      0,            // options, 输入参数，用来指定编译时的一些选项
                      &error,       // errptr, 输出参数，用来输出错误信息
                      &erroffset,   // erroffset, 输出参数，pattern中出错位置的偏移量
                      NULL);        // tableptr, 输入参数，用来指定字符表，一般情况用NULL
    // 返回值：被编译好的正则表达式的pcre内部表示结构

    int rc = pcre_exec(re,            // code, 输入参数，用pcre_compile编译好的正则表达结构的指针
                       NULL,          // extra, 输入参数，用来向pcre_exec传一些额外的数据信息的结构的指针
                       src.c_str(),           // subject, 输入参数，要被用来匹配的字符串
                       src.size(),  // length, 输入参数， 要被用来匹配的字符串的指针
                       0,             // startoffset, 输入参数，用来指定subject从什么位置开始被匹配的偏移量
                       0,             // options, 输入参数， 用来指定匹配过程中的一些选项
                       ovector,       // ovector, 输出参数，用来返回匹配位置偏移量的数组
                       OVECCOUNT);    // ovecsize, 输入参数， 用来返回匹配位置偏移量的数组的最大大小
    // 返回值：匹配成功返回非负数，没有匹配返回负数

    if (rc < 0) {
        pcre_free(re);
        return str_group = "";

    }

//    for (int i = 0; i < rc; i++) {
//        int substring_length = ovector[2 * i + 1] - ovector[2 * i]; // 字符串结束的索引 减去 开始的索引，就是字符串长度
//        string str = src.substr(ovector[2 * i] , substring_length); // 分别取出每个捕获分组
//        cout << "捕获分组: " << i << "\t" << str << endl;
//    }

    int substring_length = ovector[2 * group + 1] - ovector[2 * group]; // 字符串结束的索引 减去 开始的索引，就是字符串长度
    str_group = src.substr(ovector[2 * group] , substring_length);      // 获得捕获分组

    pcre_free(re);                     // 编译正则表达式re 释放内存
    return str_group;
}
