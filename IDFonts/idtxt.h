/***************************************************************
 * Name:      idtxt.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:     ()
 * Created:   2010-10-26
 * Copyright:  ()
 * License:
 * 使用方法: 项目构建选项-->其他选项-->填入下面两行
 -Winvalid-pch
 -include idtxt.h
 **************************************************************/

#ifndef IDTXT_H_INCLUDED
#define IDTXT_H_INCLUDED

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <ctype.h>
using namespace std;

// 获得文件长度
size_t get_file_Size(std::fstream & file);

// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath);

//  Utf-8 转 GBK
char* ConvertUtf8ToGBK(char* strUtf8);
//  Utf-8 转 GBK  string版本包装C风格字符串版本
string& ConvertUtf8ToGBK(string& strUtf8);

// 内存匹配函数memfind
char *memfind(const char *buf,const char *tofind,size_t len);

// 内存搜索，提取标记之间字符，装载到字符串容器
bool menSearch_FlagToMap(const char* buffer, size_t count,
                         const char* flag_beg, const char* flag_end,
                         map<string, size_t> &flag_map);

// 枚举搜索子键 名称和数值
void QuerySubKeyValues(HKEY hKey , map<string, string> &map_SystemFonts);

// 加载系统字体表到字体容器
bool LoadSystemFonts(map<string, string> &map_SystemFonts);

// windows控制台彩色文本函数
bool SetConsoleColor(WORD Color);

// 亮绿， 打印帮助信息
void helpinfo(const char* exefile);

#endif // IDTXT_H_INCLUDED
