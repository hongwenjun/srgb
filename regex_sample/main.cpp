#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

int main()
{
    ifstream infile("main.cpp"); // 对本CPP源文件处理
    ofstream fcout("OK.html");
    stringstream oss;
    oss << infile.rdbuf();

    string s(oss.str());
    regex reg_beg("<a href=\"http://topic\\.csdn\\.net");  // 匹配 <a href="http://topic.csdn.net
    regex reg_end("</a>");
    smatch m;
    smatch m2;

    string::const_iterator it = s.begin();
    string::const_iterator end = s.end();

    while (regex_search(it, end, m, reg_beg)) { // 以迭代器区间搜索正则式
        it = m[0].second;  // 前次匹配的末尾迭代器传给it，当下次搜索搜索的开始
        if (regex_search(m[0].first, end, m2, reg_end))
            fcout << string(m[0].first, m2[0].second) << "<p>" << endl;
        cout << string(m[0].first, m2[0].second) << "<p>" << endl;
    }
    infile.close(); fcout.close();
    return 0;
}


#if(0)  // 示例 HTML文本
{
<a href="http://topic.csdn.net/u/20111116/11/96be7f40-497f-4ab6-aacc-d3391bbbf237.html"
target="_blank">
学习分享：C++ &nbsp; 00x直接使用boost正则使用方法
</a><p>
<a href="http://topic.csdn.net/u/20110831/18/07df11c3-817f-4163-b092-e8fc04d34137.html"
target="_blank">
发个简单的代码，因为很多人 &nbsp; 分不清char &nbsp; c[] &nbsp; 和 &nbsp; char &nbsp; *c
</a><p>
<a href="http://topic.csdn.net/u/20110328/19/ace9d89a-88c7-4721-9d83-574b56c7564b.html"
target="_blank">
Code::BLOCKS &nbsp; 语法高亮 &nbsp; C/C++ &nbsp; 标准库 &nbsp; NAMESPACE &nbsp; STD &nbsp; WinAPI &nbsp; WX_API
</a><p>
}
#endif
