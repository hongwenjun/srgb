/*
Code::Blocks 学习助手 V0.18 By 蘭公子
本工具切换VC2010中英文，或者开启命令行的编译器环境
Debug模式，命令行后台输出调试信息，但是不能显示图片
Release模式，可以显示图片和图标，不显示命令行后台
*/
#include "cbstudy.h"  //加载预编译头文件

//INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CBStudyInitdialog(HWND & hwnd);
void CBStudyReadINI(HWND & hwnd);


HBITMAP g_hBitmap1;	// 第一个图片的句柄
HBITMAP g_hBitmap2;	// 第二个图片的句柄
HICON	g_hIcon;	// 对话框图标句柄
HBRUSH	g_hBgBrush;	// 背景刷子


//定义路径全局变量存储
bool Change_PIC = false ;
char CBS_vcbin[MAX_PATH], CBS_include[MAX_PATH], CBS_lib[MAX_PATH],
     CBS_PATH[MAX_PATH], CBS_gccbin[MAX_PATH];

string strCBS_Cmdline , strCBS_CN2052, strCBS_EN1033, strCBS_TMP;
char * cn2052 , * en2052 ;



// Windows系统的主函数
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd)
{

    // 从资源中加载BMP文件和图标，这些工作也可以在WM_INITDIALOG消息中进行
    g_hBitmap1 = ::LoadBitmap(hInst, (LPCTSTR)IDB_BITMAP1);
    g_hBitmap2 = ::LoadBitmap(hInst, (LPCTSTR)IDB_BITMAP2);
    g_hIcon = ::LoadIcon(hInst, (LPCTSTR)IDI_ICON);

    // 用这个系统函数创建我们刚刚绘制好的窗体DLG_MAIN，设置回叫函数为DlgProc
    DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN ), 0, DlgProc);

    return 0;
}


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


    // 对消息类别进行判断
    switch(uMsg) {

    case WM_INITDIALOG: {

        CBStudyInitdialog(hwnd); // 设置标题栏图标,// 设置图片
        CBStudyReadINI(hwnd);    // 读取 CBStudy.ini 配置


    }
    break;

    // 如果是命令事件（按钮等）
    case WM_COMMAND:
        // 对wParam低字节进行判断，其中是我们的ID（参见MSDN）
        switch(LOWORD(wParam)) {

        case IDC_Change_PIC:
            if (!Change_PIC) {
                SetDlgItemText(hwnd, IDC_INFO_TEXT, "美女都让你发现了，你真厉害！");
                ::SendDlgItemMessage(hwnd, IDC_BABY_PIC, STM_SETIMAGE, IMAGE_BITMAP, (long)g_hBitmap2);
                printf("改图片\n");
                Change_PIC = true;
            } else {
                CBStudyInitdialog(hwnd);
                Change_PIC = false;
            }
            break;
        case IDC_CLUI_CN:

            strCBS_TMP = CBS_vcbin;
            strCBS_CN2052 = CBS_PATH;
            strCBS_CN2052 += strCBS_TMP;
            strCBS_EN1033 = strCBS_CN2052 + "\\EN2052";
            strCBS_CN2052 += "\\2052";

            cn2052 =new char[MAX_PATH];
            en2052 =new char[MAX_PATH];
            strcpy(cn2052, strCBS_CN2052.c_str());
            strcpy(en2052, strCBS_EN1033.c_str());

            MoveFile(en2052,cn2052);
            printf("%s\n",cn2052);
            printf("%s\n",en2052);

            delete[] cn2052;
            delete[] en2052;

            break;

        case IDC_CLUI_EN:

            strCBS_TMP = CBS_vcbin;
            strCBS_CN2052 = CBS_PATH;
            strCBS_CN2052 += strCBS_TMP;
            strCBS_EN1033 = strCBS_CN2052 + "\\EN2052";
            strCBS_CN2052 += "\\2052";

            cn2052 =new char[MAX_PATH];
            en2052 =new char[MAX_PATH];
            strcpy(cn2052, strCBS_CN2052.c_str());
            strcpy(en2052, strCBS_EN1033.c_str());

            MoveFile(cn2052, en2052);
            printf("%s\n",cn2052);
            printf("%s\n",en2052);

            delete[] cn2052;
            delete[] en2052;

            break;

        case IDC_CLUI_VCCMD:

            strCBS_TMP = CBS_vcbin;
            strCBS_Cmdline = CBS_PATH;
            strCBS_Cmdline += strCBS_TMP;
            printf("%s\n",strCBS_Cmdline.c_str());

            break;

        case IDC_CLUI_GCCMD:
            printf("命令行VC2010编译器\nVC编译器路径 %s\n",CBS_gccbin);
            break;

            // 如果是Cancel按钮被按下
        case IDC_BTN_QUIT :
            // 这里读者可以充分发挥想象力，控制台函数可以调用 :)

            if (Change_PIC)
                ::EndDialog (hwnd, IDC_BTN_QUIT);

            // 分配空间作为临时存储
            char buf[512];

            // 获得Edit Control内容
            GetDlgItemText(hwnd, IDC_INFO_TEXT, buf, 512);

            // 用printf打印我们获得的内容
            printf("%s\n", buf);

            SetDlgItemText(hwnd, IDC_INFO_TEXT, "菜鸟，想退出了吗！程序中有个彩蛋等你发现！");
            Change_PIC = true;

            break;
        }
        break;

        // 如果窗口被关闭
    case WM_CLOSE:
        // 要求系统关闭这个程序
        PostQuitMessage(0);
        break;
    }

    // 默认返回0
    return 0;
}

void CBStudyInitdialog(HWND & hwnd)
{

    // 设置标题栏图标
    ::SendMessage(hwnd, WM_SETICON, ICON_BIG, (long)g_hIcon);

    // 初始化显示图片的静态框架
    HWND hWndBmp = ::GetDlgItem(hwnd, IDC_BABY_PIC );
    // 设置SS_BITMAP风格
    LONG nStyle = ::GetWindowLong(hWndBmp, GWL_STYLE);
    ::SetWindowLong(hWndBmp, GWL_STYLE, nStyle | SS_BITMAP);
    // 设置图片
    ::SendDlgItemMessage(hwnd, IDC_BABY_PIC, STM_SETIMAGE, IMAGE_BITMAP, (long)g_hBitmap1);
    SetDlgItemText(hwnd, IDC_INFO_TEXT, "本工具切换VC2010中英文，或者开启命令行的编译器环境");
}

#include "cbstudy.h"


void CBStudyReadINI(HWND & hwnd)
{
    GetCurrentDirectoryA(MAX_PATH, CBS_PATH);

    GetPrivateProfileString("VC2010_PATH", "VCBIN", "VCBIN", CBS_vcbin, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("VC2010_PATH", "INCLUDE", "INCLUDE", CBS_include, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("VC2010_PATH", "LIB", "LIB", CBS_lib, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("GCC_PATH", "GCCBIN", "GCCBIN", CBS_gccbin, MAX_PATH,".\\CBStudy.ini");
}

