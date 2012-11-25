#ifndef PKFILE_H_INCLUDED
#define PKFILE_H_INCLUDED

/*
** Copyright (C) Http://sRGB.GoogleCode.com All rights reserved.
*/
#include "pk_pch.h"

// 简单的文件属性结构
struct F_STRUCT {
    char date[16];
    char time[16];
    char size[255];
    char name[255];
};

// 参数初始化
int initial_arg(int argc , char* argv[]);

// 删除字符串前后空白
string strTrim(string& str);

// 读取和保存配置
void LoadConfigFile();

// 打印帮助信息
void print_help();

// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName);

// 功能 获得当前路径
char* GetAppDir(char* szPath);



#endif // PKFILE_H_INCLUDED


