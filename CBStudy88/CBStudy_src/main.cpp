/*
Code::Blocks 学习助手 V0.18 By 蘭公子
本工具切换VC2010中英文，或者开启命令行的编译器环境
Debug模式，命令行后台输出调试信息，但是不能显示图片
Release模式，可以显示图片和图标，不显示命令行后台
*/
#include "cbstudy.h"  //加载预编译头文件





INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CBStudyInitdialog(HWND & hwnd);
void CBStudyReadINI();    //读取配置模块

bool Clui_Language(int CodePage);    // 设置VC编译器语言代码页
bool CBSfullPath(char* CBS_PATH, char* mypath);  // 转换绝对路径
bool ConsoleCompiler(const char * ch);

HBITMAP g_hBitmap1;	// 第一个图片的句柄
HBITMAP g_hBitmap2;	// 第二个图片的句柄
HICON	g_hIcon;	// 对话框图标句柄
HBRUSH	g_hBgBrush;	// 背景刷子


//定义路径全局变量存储
bool Change_PIC = false ;
char CBS_vcbin[MAX_PATH],  CBS_include[MAX_PATH], CBS_lib[MAX_PATH],
     CBS_gccbin[MAX_PATH], CBS_gccnls[MAX_PATH],
     CBS_PATH[MAX_PATH],   CBS_MYAPP[MAX_PATH];

string strCBS_Cmdline ;

// Windows系统的主函数
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd)
{

    // 从资源中加载BMP文件和图标，这些工作也可以在WM_INITDIALOG消息中进行
    g_hBitmap1 = ::LoadBitmap(hInst, (LPCTSTR)IDB_BITMAP1);
    g_hBitmap2 = ::LoadBitmap(hInst, (LPCTSTR)IDB_BITMAP2);
    g_hIcon = ::LoadIcon(hInst, (LPCTSTR)IDI_ICON);

    CBStudyReadINI();    // 读取 CBStudy.ini 的路径配置

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
            Clui_Language(2052); //设置中文代码页2052
            SetDlgItemText(hwnd, IDC_INFO_TEXT,  "您已经切换C/C++编译器构建信息为中文!");
            break;

        case IDC_CLUI_EN:
            Clui_Language(1033); //设置中文代码页1033
            SetDlgItemText(hwnd, IDC_INFO_TEXT,  "C/C++ compiler build information for the English!");

            break;

        case IDC_CLUI_VCCMD:
            printf("VC编译器路径 %s\n",CBS_vcbin);
            ConsoleCompiler("vc");
            break;

        case IDC_CLUI_GCCMD:
            printf("GCC编译器路径 %s\n",CBS_gccbin);
            ConsoleCompiler("gcc");
            break;

            // 如果是Cancel按钮被按下
        case IDC_BTN_QUIT :
            // 这里读者可以充分发挥想象力，控制台函数可以调用 :)

            if (Change_PIC) {
                ::EndDialog (hwnd, IDC_BTN_QUIT);
                DeleteFile("CBStudy.cmd");
            }

            // 分配空间作为临时存储
            char buf[512];

            // 获得Edit Control内容
            GetDlgItemText(hwnd, IDC_INFO_TEXT, buf, 512);

            // 用printf打印我们获得的内容
            printf("%s\n", buf);

            SetDlgItemText(hwnd, IDC_INFO_TEXT, "菜鸟，想退出了吗！程序中还有个彩蛋等你发现呢！");
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
    SetDlgItemText(hwnd, IDC_INFO_TEXT, "本工具切换C++编译器中英文，开启命令行的编译器环境");
}

void CBStudyReadINI()
{
    //读取配置文件路径
    GetPrivateProfileString("VC2010_PATH", "VCBIN", "VCBIN", CBS_vcbin, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("VC2010_PATH", "INCLUDE", "INCLUDE", CBS_include, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("VC2010_PATH", "LIB", "LIB", CBS_lib, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("GCC_PATH", "GCCBIN", "GCCBIN", CBS_gccbin, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("GCC_PATH", "GCCNLS", "C:\\gcc\\share\\locale\\zh_CN", CBS_gccnls, MAX_PATH,".\\CBStudy.ini");
    GetPrivateProfileString("MYAPP_PATH", "MYAPP", "E:\\myapp", CBS_MYAPP, MAX_PATH,".\\CBStudy.ini");

    // 转为为绝对路径
    GetCurrentDirectoryA(MAX_PATH, CBS_PATH);
    CBSfullPath(CBS_PATH, CBS_vcbin);
    CBSfullPath(CBS_PATH, CBS_include);
    CBSfullPath(CBS_PATH, CBS_lib);
    CBSfullPath(CBS_PATH, CBS_gccbin);
    CBSfullPath(CBS_PATH, CBS_gccnls);
    CBSfullPath(CBS_PATH, CBS_MYAPP);
//  printf("%s\n%s\n",CBS_gccnls,CBS_MYAPP);
}

bool CBSfullPath(char* CBS_PATH, char* mypath)
{
    string strCBS(CBS_PATH);
    string absPath(mypath);
    string relativePath(mypath, 1, MAX_PATH - 3);
    if(mypath[0] == '.') {
        absPath = strCBS + relativePath;
        strcpy(mypath, absPath.c_str());
    }
    return true;
}

bool Clui_Language(int CodePage)
{
    string strCBS_CN2052(CBS_vcbin);   // VC 中文语言路径
    string strCBS_EN2052(CBS_vcbin);
    strCBS_CN2052 += "\\2052";
    strCBS_EN2052 += "\\EN2052";
    char *cn2052 , *en2052 ;

    string strCBS_CNnls(CBS_gccnls);  // GCC 中文语言路径
    string strCBS_ENnls(CBS_gccnls);
    strCBS_ENnls += "_OFF";
    char *cnnls , *ennls ;

    cn2052 =new char[MAX_PATH];       // string 转到 char指针，让后面MoveFile()调用
    en2052 =new char[MAX_PATH];
    cnnls =new char[MAX_PATH];
    ennls =new char[MAX_PATH];
    strcpy(cn2052, strCBS_CN2052.c_str());
    strcpy(en2052, strCBS_EN2052.c_str());
    strcpy(cnnls, strCBS_CNnls.c_str());
    strcpy(ennls, strCBS_ENnls.c_str());

    if (2052 == CodePage)  {     //中文
        MoveFile(en2052, cn2052);
        MoveFile(ennls, cnnls);
    }
    if (1033 == CodePage) {       //英文
        MoveFile(cn2052, en2052);
        MoveFile(cnnls, ennls);
    }
    delete[] cn2052;
    delete[] en2052;
    delete[] cnnls;
    delete[] ennls;

    printf("%i\n", CodePage);
    return true;
}

bool ConsoleCompiler(const char * ch)
{
    std::ofstream fout( "CBStudy.cmd" );       // 建立批处理文件
    if ('v'==ch[0]) {
        fout << "@echo off\nset PATH=" << CBS_vcbin << ";%PATH%\nset INCLUDE=" <<CBS_include
             <<"\nset LIB=" << CBS_lib <<"\ncolor a\n@echo 欢迎使用命令行VC2010编译器中文版  你可以使用TAB自动补全\ncl\n" ;
    }
    if ('g'==ch[0]) {
        fout << "@echo off\nset PATH=" << CBS_gccbin << ";%PATH%\ncolor a\n@echo 欢迎使用命令行 GCC 编译器中文版  你可以使用TAB自动补全\ng++ -v\n" ;
    }
    fout << "CD " << CBS_MYAPP << endl;
    fout.close();

    // 执行批处理文件
    char szCommandLine[] = "cmd /k CBStudy.cmd";
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;	// 指定wShowWindow成员有效
    si.wShowWindow = TRUE;			// 此成员设为TRUE的话则显示新建进程的主窗口，
    // 为FALSE的话则不显示
    BOOL bRet = ::CreateProcess (
                    NULL,			// 不在此指定可执行文件的文件名
                    szCommandLine,		// 命令行参数
                    NULL,			// 默认进程安全性
                    NULL,			// 默认线程安全性
                    FALSE,			// 指定当前进程内的句柄不可以被子进程继承
                    CREATE_NEW_CONSOLE,	// 为新进程创建一个新的控制台窗口
                    NULL,			// 使用本进程的环境变量
                    NULL,			// 使用本进程的驱动器和目录
                    &si,
                    &pi);
    return bRet;
}
