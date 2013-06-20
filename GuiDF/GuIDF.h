#ifndef GUIDF_H_INCLUDED
#define GUIDF_H_INCLUDED

#include <windows.h>
#include  <shlobj.h>
#include <Setupapi.h>
#include "resource.h"
#include <fstream>
#include "xcgui.h"

#pragma warning(disable: 4996)


using namespace std;



// 读取和保存配置
void LoadConfigFile();
void SaveConfigFile();
// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName);
// 功能 获得当前路径
char* GetAppDir(char* szPath);

// 执行命令行
int execute_command(char* cmdline);
// 选择一个文件
int   GetFilePath(HWND hWnd, char *szFile);
// 选择一个目录
int   GetPath(HWND hWnd, char *pBuffer);

// 路径转宽字节
WCHAR* charToWCHAR(WCHAR* wch, char* czs);
char* WCHARTochar(char* czs , WCHAR* wch);
// 窗口布局
void InitXC_Window(HWINDOW& hWindow);

// 事件响应
bool CALLBACK appBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK docBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK fontBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK closeBtnClick(HELE hEle, HELE hEventEle);


#endif // GUIDF_H_INCLUDED
