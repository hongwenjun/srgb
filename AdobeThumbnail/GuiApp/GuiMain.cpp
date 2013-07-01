#include "GuiThumbnail.h"
#include "GuiMain.h"
#include <wchar.h>

// 全局变量
char ConfigFile[MAX_PATH] = {0};
WCHAR docFile[MAX_PATH] =  L"2KC AI";
WCHAR savePath[MAX_PATH] = L"D:\\Thumbs";

// 全局句柄
HELE  keyEdit, pathEdit , hList;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    XInitXCGUI(); // 初始化
    Everything_Start();

    extern char ConfigFile[MAX_PATH];
    GetAppDir(ConfigFile);
    strcat(ConfigFile, "/ACThumbs.ini");
    if (IsFileExist(ConfigFile))
        LoadConfigFile();

    HWINDOW hWindow = XWnd_CreateWindow(0, 0, 800, 600, L"Adobe AI EPS INDD 和CorelDRAW 缩略图收集工具  版权所有 2013.06 Hongwenjun (蘭公子)"); // 创建窗口
    if (hWindow) {
        // 设置图标
        HICON logo_hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
        XWnd_SetIcon(hWindow, logo_hIcon, false);
        InitXC_Window(hWindow); // 窗口布局

        XWnd_ShowWindow(hWindow, SW_SHOW); //显示窗口
        XRunXCGUI(); //运行
    }
    return 0;
}


// 读取和保存配置
void LoadConfigFile()
{
    wfstream wFile(ConfigFile , fstream::in);
    wFile.imbue(locale("chs"));
    wFile.getline(docFile, MAX_PATH);
    wFile.getline(savePath, MAX_PATH);
    wFile.close();
}
void SaveConfigFile()
{
    wfstream wFile(ConfigFile , fstream::out);
    wFile.imbue(locale("chs"));
    wFile << docFile << endl << savePath << endl;
    wFile.close();
}


// 文本框回写配置
bool edit_text(void)
{
    XEdit_GetText(keyEdit, docFile, MAX_PATH);
    XEdit_GetText(pathEdit, savePath, MAX_PATH);

    return true;
}

// 窗口布局
void InitXC_Window(HWINDOW& hWindow)
{
//   XWnd_SetImage(hWindow, XImage_LoadFile(L"GuiBG.dll")); // 设置窗口背景图片

    // 创建编辑框
    keyEdit = XEdit_Create(1, 2, 302, 22, hWindow);
    pathEdit = XEdit_Create(412, 2, 270, 22, hWindow);

    // 读取配置到文本框
    XEdit_SetText(keyEdit, docFile);
    XEdit_SetText(pathEdit, savePath);


    // 工具提示按钮
    HELE keyButton = XBtn_Create(306, 2, 98, 22,  L"关键字 --> 搜索", hWindow);
    XEle_EnableToolTips(keyButton, true);
    XEle_SetToolTips(keyButton, L"请输入关键字,用空格隔开");

    HELE pathButton = XBtn_Create(685, 2, 98, 22, L"缩略图保存目录", hWindow);
    XEle_EnableToolTips(pathButton, true);
    XEle_SetToolTips(pathButton, L"选择缩略图保存的目录");

    HELE runButton = XBtn_Create(685, 30, 88, 32, L"开始执行", hWindow);
    XEle_EnableToolTips(runButton, true);
    XEle_SetToolTips(runButton, L"开始执行提取缩略图");
    HELE closeButton = XBtn_Create(685, 70, 88, 32, L"保存关闭", hWindow);

//    // 注册文本框输入事件
//    XEle_RegisterEvent(keyEdit, XE_EDIT_CHANGE, keyEditChange);

    // 注册按钮事件
    XEle_RegisterEvent(keyButton, XE_BNCLICK, keyBtnClick);
    XEle_RegisterEvent(pathButton, XE_BNCLICK, pathBtnClick);
    XEle_RegisterEvent(runButton, XE_BNCLICK, runBtnClick);
    XEle_RegisterEvent(closeButton, XE_BNCLICK, closeBtnClick);


    //创建列表元素
    hList = XList_Create(1, 25, 650, 518, hWindow);

    //添加列表头
    XList_AddColumn(hList, 260, L"名称", 2);
    XList_AddColumn(hList, 260, L"路径", 2);
    XList_AddColumn(hList, 100, L"文件版本", 2);

    // 创建静态文本
    HELE hStatic = XStatic_Create(1, 545, 650, 22, L"Adobe AI EPS INDD 和CorelDRAW 缩略图收集工具  版权所有 2013.06 Hongwenjun (蘭公子)", hWindow);


    //创建图片元素
    HELE hPic1 = XPic_Create(654, 416, 128, 128, hWindow);
    XPic_SetImage(hPic1, XImage_LoadFile(L"Thumbnail.dat")); //设置显示图片
    HELE hPic2 = XPic_Create(654, 416 - 130, 128, 128, hWindow);
    XPic_SetImage(hPic2, XImage_LoadFile(L"Thumbnail.dat")); //设置显示图片
    HELE hPic3 = XPic_Create(654, 416 - 130 - 130, 128, 128, hWindow);
    XPic_SetImage(hPic3, XImage_LoadFile(L"Thumbnail.dat")); //设置显示图片
}


// 事件响应

//bool CALLBACK keyEditChange(HELE hEle, HELE hEventEle)
//{
//    return true;
//}

bool CALLBACK keyBtnClick(HELE hEle, HELE hEventEle)
{
    wchar_t wbuf[MAX_PATH]={0};
    XEdit_GetText(keyEdit, wbuf, MAX_PATH);

    Everything_SetSearch(wbuf);
    bool EQRet =  Everything_Query(TRUE);
    if (EQRet == false) {
        return false;
    }

    // 删除所有项目
    XList_DeleteAllItems(hList);

    //添加列表项      // Display results.
   for (int i = 0 ; i < Everything_GetNumResults(); i++){
        //添加项
        XList_AddItem(hList, (wchar_t *)Everything_GetResultFileName(i), 0);
            //设置子项内容
            XList_SetItemText(hList, i, 1, (wchar_t *)Everything_GetResultPath(i), 1);
        //    XList_SetItemText(hList, i, 2, wbuf, 1);

    }
    return true;


}

bool CALLBACK pathBtnClick(HELE hEle, HELE hEventEle)
{
    char buf[MAX_PATH];
    if (GetPath(NULL , buf))
        charToWCHAR(savePath, buf);
    XEdit_SetText(pathEdit, savePath);
    return true;
}

bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle)
{

    // 文本框回写配置
    edit_text();

    // 格式化命令行
    wchar_t wbuf [2 * MAX_PATH] = {0};
    char cmdline[2 * MAX_PATH] = {0};
    swprintf(wbuf, L"\"%s\" \"%s\"" , docFile, savePath);
    WCHARTochar(cmdline, wbuf);

    // MessageBoxA(NULL, cmdline, "Debug", MB_OKCANCEL);
    //  execute_command(cmdline);

    // 打开记录文件
    ShellExecute(NULL, "open", "使用字体报告.txt", NULL, NULL, SW_SHOW);

    return true;
}

bool CALLBACK closeBtnClick(HELE hEle, HELE hEventEle)
{
    // 文本框回写配置
    edit_text();

    SaveConfigFile();
    ExitProcess(0); // 退出程序
    return true;
}

// 选择一个目录
int   GetPath(HWND hWnd, char* pBuffer)
{
    BROWSEINFO   bf;
    LPITEMIDLIST   lpitem;
    memset(&bf, 0, sizeof(BROWSEINFO));
    bf.hwndOwner = hWnd;
    bf.lpszTitle = L"选择路径 ";
    bf.ulFlags = BIF_RETURNONLYFSDIRS;   //属性你可自己选择
    lpitem = SHBrowseForFolder(&bf);
    if (lpitem == NULL)  //如果没有选择路径则返回   0
        return  0;

    //如果选择了路径则复制路径,返回路径长度

    SHGetPathFromIDListA(lpitem, pBuffer);
    return   lstrlen(pBuffer);
}


bool Everything_Start(void)
{
    Everything_SetSearch(L"Everything.exe");
    bool EQRet =  Everything_Query(TRUE);
    if (EQRet == false) {

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        // 后台隐藏
        si.dwFlags   =   STARTF_USESHOWWINDOW;
        si.wShowWindow   =   SW_HIDE;
        ZeroMemory(&pi, sizeof(pi));
        // Start the child process.
        CreateProcess(NULL, "Everything.exe", NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi);
    }
    return true;
}
