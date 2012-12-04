#ifndef PKFILE_H_INCLUDED
#define PKFILE_H_INCLUDED

/*
** Copyright (C) Http://sRGB.GoogleCode.com All rights reserved.
*/
#include "pk_pch.h"

// 简单的文件属性结构
struct F_STRUCT {
    char date[16];
    char time[12];
    size_t size;
    char name[MAX_PATH];
    char path[MAX_PATH];
};

// 参数初始化
int initial_arg(int argc , char* argv[]);

// 删除字符串前后空白
string strTrim(string& str);
// 删除字符串前后空白
char* csTrim(char* cs);


// 读取和保存配置
void LoadConfigFile();

// 打印帮助信息
void print_help();

// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName);

// 功能 获得当前路径
char* GetAppDir(char* szPath);


// 搜索lp_path目录包括子目录所有文件名，保存到输出out_file_name ; 返回打印的记录数
int find_path_save_file(const char* lp_path , const char* out_file_name) ;
// 遍历搜索目录
int find_path(const char* lp_path);

#endif // PKFILE_H_INCLUDED


