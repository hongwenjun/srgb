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
bool DEBUG_MODE = 0;


// STL中用min_element/max_element返回map的最值元素
// http://chice.bokee.com/4105110.html
template <class fT, class sT>
struct second_less : public binary_function <fT , sT , bool> {
    bool operator()(
        const pair<fT, sT>& _1,
        const pair<fT, sT>& _2
    ) const
    {
        return _1.second < _2.second;
    }
};

int set_srgb_x64_me_ip(const string& ipaddr);
int set_srgb_vicp_net_ip(const string& ipaddr);

std::string& regex_get_group(const std::string& src , const std::string& pattern ,
                             int group , std::string& str_group);

std::string& load_string(std::string& str, fstream& infile)  // 加载文件到string
{
    std::stringstream ss;
    ss << infile.rdbuf();
    str = ss.str();
    return str;
}

void initializer_webdata(void);
//  g++ -std=c++0x -lpcre -o getwanip   编译选项

int main(int argc, char* argv[])
{
    string option_url;
    for (int i = 0 ; i != argc ; i++)
        option_url = option_url + " " + argv[i] ;

    if (option_url.find("-d") != string::npos)
        DEBUG_MODE = 1;

    if (option_url.find("http") != string::npos) {
        string cmdline = "curl -s ";
        cmdline += option_url.substr(option_url.find("http"));
        cmdline += " >> /tmp/getwanip.txt";
        system("touch /tmp/getwanip.txt");
        system(cmdline.c_str());

        if (DEBUG_MODE) {
            cout << cmdline << endl;
            system("cat /tmp/getwanip.txt");
            cout << "\n----------------------初始化网络IP数据完成！----------------------\n";
        }
    } else
        initializer_webdata();

    std::fstream fs("/tmp/getwanip.txt", std::fstream::in);
    std::string  src;  // 收集的网络 wanip数据
    std::string  reg  = "(\\d+\\.\\d+\\.\\d+\\.\\d+)";     // 简单匹配IP地址正则公式
    load_string(src, fs);
    fs.close();
    remove("/tmp/getwanip.txt");

    std::string ipaddr; int cnt = 0;
    regex_get_group(src, reg , 1 , ipaddr);
    if (ipaddr == "")
        return -1;

    std::map<string, int> map_wanip;
    while (ipaddr != "") {
        if (DEBUG_MODE)
            cout << cnt++ << ": " << ipaddr << endl;

        ++map_wanip[ipaddr];
        src = src.substr(src.find(ipaddr) + ipaddr.size());
        regex_get_group(src, reg , 1 , ipaddr);
    }

    // 找到出现最多的IP，计数器
    auto it = std::max_element(map_wanip.begin(), map_wanip.end(), second_less<const string, int>());
    cout << it->first << endl;

    // 自动设置 srgb.x64.me 网址密码
    int ret = set_srgb_x64_me_ip(it->first);
    ret += set_srgb_vicp_net_ip(it->first);
    return ret;
}

int set_srgb_x64_me_ip(const string& ipaddr)
{
    std::fstream fs("/tmp/srgb.x64.me", std::fstream::in);
    std::string  srgb_x64_me_ip;
    load_string(srgb_x64_me_ip, fs);
    fs.close();

    if (srgb_x64_me_ip.find(ipaddr) != string::npos) {
        std::fstream fs("/tmp/srgb.x64.me", std::fstream::out);
        fs << "sRGB.x64.me WAN IP: " << ipaddr << " good 不用更新!" << endl;
        fs.close();
        return 1;
    }

    if (srgb_x64_me_ip.find("nochg") != string::npos) {
        std::fstream fs("/tmp/srgb.x64.me", std::fstream::out);
        fs << "sRGB.x64.me WAN IP: " << ipaddr << " good 不用更新!" << endl;
        fs.close();
        return 2;
    }

//  https://username:password@www.dnsdynamic.org/api/?hostname=techno.ns360.info&myip=127.0.0.1
    string cmdline = "wget --http-user=username   --http-passwd=password --no-check-certificate -q -O /tmp/srgb.x64.me "
                     "https://www.dnsdynamic.org/api/?hostname=srgb.x64.me\\&myip=";
    cmdline += ipaddr;
    system(cmdline.c_str());
    system("cat /tmp/srgb.x64.me");

    return 0;
}

int set_srgb_vicp_net_ip(const string& ipaddr)
{
    std::fstream fs("/tmp/srgb.vicp.net", std::fstream::in);
    std::string  srgb_vicp_net_ip;
    load_string(srgb_vicp_net_ip, fs);
    fs.close();

    if (srgb_vicp_net_ip.find(ipaddr) != string::npos) {
        std::fstream fs("/tmp/srgb.vicp.net", std::fstream::out);
        fs << "sRGB.vicp.net WAN IP: " << ipaddr << " good 不用更新!" << endl;
        fs.close();
        return 1;
    }

    if (srgb_vicp_net_ip.find("nochg") != string::npos) {
        std::fstream fs("/tmp/srgb.vicp.net", std::fstream::out);
        fs << "sRGB.vicp.net WAN IP: " << ipaddr << " good 不用更新!" << endl;
        fs.close();
        return 2;
    }

//    使用URL验证,适用于浏览器或应用程序（fetch, curl, lwp-request），可以在URL中包含验证信息。
//    http://username:password@ddns.oray.com/ph/update?hostname=yourhostname&myip=ipaddress

    string cmdline = "wget --http-user=username   --http-passwd=password  -q -O /tmp/srgb.vicp.net "
                     "http://ddns.oray.com/ph/update?hostname=srgb.vicp.net\\&myip=";

    cmdline += ipaddr;
    system(cmdline.c_str());
    system("cat /tmp/srgb.vicp.net");

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


void initializer_webdata(void)
{
    system("touch /tmp/getwanip.txt");
    system("curl -s 192.168.1.1 | grep ipinfo >>/tmp/getwanip.txt");
    system("curl -s http://www.ip138.com/ips1388.asp | grep ip_add.asp  >>/tmp/getwanip.txt");
    system("curl -s http://ns1.dnspod.net:6666    >>/tmp/getwanip.txt");
    system("echo \"  \"  >>/tmp/getwanip.txt");
    system("curl -s http://myip.dnsdynamic.org/  >>/tmp/getwanip.txt");

    if (DEBUG_MODE) {
        system("cat /tmp/getwanip.txt");
        cout << "\n----------------------初始化网络IP数据完成！----------------------\n";
    }
}
