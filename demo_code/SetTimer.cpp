#include <Windows.h>
#include <stdio.h>
#include <conio.h>

int count = 0;
WORD Color = 0x89;
//   http://blog.csdn.net/hongwenjun/article/details/6202127
//   windows控制台彩色文本函数，今天研究了别人，摸索出使用方法了

bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor);
bool SetConsoleColor(WORD Color);
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
DWORD CALLBACK Thread(PVOID pvoid);


int main()
{
    DWORD dwThreadID;
    printf("use timer in workthread of console application\n");
    HANDLE hThread = CreateThread(NULL, 0, Thread, NULL, 0, NULL);
    getchar();
    system("cls");
    printf("秒数: %d", count);
    return 0;
};


void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{

    SetConsoleColor(Color += 0x16); // 改变背景颜色
    // system("cls"); 如果要整屏改变，要清屏
    printf("Time:%d%s\n", count++ , "\t\t\t\t\t\t\t\t\t\t\b");
}

DWORD CALLBACK Thread(PVOID pvoid)
{
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    UINT timerid = SetTimer(NULL, 111, 1000, TimerProc);
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            printf("Error:the thread will quit,error id is %d\n", GetLastError());
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    KillTimer(NULL, timerid);
    printf("thread end here\n");
    return 0;
}

bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor)
{
    // 获得缓冲区句柄。参数可以选择一下三种值：
    //// STD_INPUT_HANDLE            标准输入的句柄
    //// STD_OUTPUT_HANDLE           标准输出的句柄
    //// STD_ERROR_HANDLE            标准错误的句柄
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == 0)
        return false;
    //设置文本及背景颜色。
    BOOL ret = SetConsoleTextAttribute(handle, forceGroundColor | backGroundColor);
    return (ret == TRUE);
}
bool SetConsoleColor(WORD Color)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == 0)
        return false;
    BOOL ret = SetConsoleTextAttribute(handle, Color);
    return (ret == TRUE);
}
