#ifndef GUIMAIN_H_INCLUDED
#define GUIMAIN_H_INCLUDED

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


// 选择一个文件
int   GetFilePath(HWND hWnd, char *szFile);
// 选择一个目录
int   GetPath(HWND hWnd, char *pBuffer);


// 事件响应
bool CALLBACK keyEditChange(HELE hEle, HELE hEventEle);
bool CALLBACK keyBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK pathBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle);
bool CALLBACK closeBtnClick(HELE hEle, HELE hEventEle);


// 窗口布局
void InitXC_Window(HWINDOW& hWindow);

bool Everything_Start(void);
#endif // GUIMAIN_H_INCLUDED
