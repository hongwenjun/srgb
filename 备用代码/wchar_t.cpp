#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

int main()
{
    setlocale(LC_ALL, "");     // 用这句 GCC 和 VC编译器都支持输出宽字节，下面这句只支持VC
    //  wcout.imbue(locale( "chs"));  // 需要设置imbue才能输出中文字符 // set locale to argument

    FILE* wfp = fopen("gbk.txt" , "r");

    wchar_t wch;
    map<wchar_t , int> mci; // 装载字母计数

    while (fwscanf(wfp, L"%c", &wch)) {  // GCC支持，VC好像又不支持了
        ++mci[wch];  // 计数器加1
    }
    for (auto it = mci.begin(); it != mci.end(); it++)
        wcout << (*it).first << " => " << (*it).second << "\n";
}

/* GCC 不支持以下  C++流的宽字节输入
    wifstream inFile("gbk.txt");
    inFile.imbue(locale("chs"));  // 需要设置imbue才能输入GBK格式文件的中文字符
    while (inFile >> wch)
        ++mci[wch];  // 计数器加1
*/
