#ifndef PYTOOL_H_INCLUDED
#define PYTOOL_H_INCLUDED

//Code::Blocks 编译器参数
//-fexec-charset=gbk
//-finput-charset=UTF-8
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
using namespace std;

struct PINYIN {
    wchar_t hz;
    wstring pyyb;
    int multiTone;
};

wstring& abc2py(wstring& str); // 将字母替换成拼音符号
char* GetAppDir(char* szPath);  // 功能 获得当前路径
bool InitPinyindb(map<wchar_t, wstring>& pyTable); // 装载拼音表
void AppHelp(); // 帮助

#endif // PYTOOL_H_INCLUDED

