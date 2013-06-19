#include "GuIDF.h"

// 全局变量
char ConfigFile[MAX_PATH] = {0};
WCHAR appFile[MAX_PATH] =  L"IDFonts.exe";
WCHAR docFile[MAX_PATH] =  L"test.indd";
WCHAR fontPath[MAX_PATH] = L"D:\\Fonts";

// 全局句柄
HELE appEdit , docEdit, fontEdit;

// 读取和保存配置
void LoadConfigFile()
{
    wfstream wFile(ConfigFile , fstream::in);
    wFile.imbue(locale("chs"));
    wFile.getline(appFile, MAX_PATH);
    wFile.getline(docFile, MAX_PATH);
    wFile.getline(fontPath, MAX_PATH);
    wFile.close();
}
void SaveConfigFile()
{
    wfstream wFile(ConfigFile , fstream::out);
    wFile.imbue(locale("chs"));
    wFile << appFile << endl << docFile << endl << fontPath << endl;
    wFile.close();
}

// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName)
{
    WIN32_FIND_DATA fd = {0};
    HANDLE hFind = FindFirstFile(lpFileName, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    }
    return ((hFind != INVALID_HANDLE_VALUE) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

// 功能 获得当前路径
char* GetAppDir(char* szPath)
{
    char* ret = szPath;
    int i;
    GetModuleFileName(NULL, szPath, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    i = strlen(szPath);
    while ((i > 0) && (szPath[i - 1] != '\\')) { // 删除文件名，只留下目录
        szPath[--i] = 0;
    }
    return ret;
}

// 窗口布局
void InitXC_Window(HWINDOW& hWindow)
{
    XWnd_SetImage(hWindow, XImage_LoadFile(L"GuiBG.dll")); // 设置窗口背景图片

    HELE hStatic = XStatic_Create(7, 10, 390, 18, L"用法: IDFonts.exe  InDesign_Doc.indd  [SaveFontPath]", hWindow);
    //创建静态文本元素
    XEle_SetBkTransparent(hStatic, true); //设置背景透明

    // 创建编辑框
    appEdit = XEdit_Create(5, 30, 270, 22, hWindow);
    docEdit = XEdit_Create(5, 60, 270, 22, hWindow);
    fontEdit = XEdit_Create(5, 90, 270, 22, hWindow);

    // 读取配置到文本框
    XEdit_SetText(appEdit, appFile);
    XEdit_SetText(docEdit, docFile);
    XEdit_SetText(fontEdit, fontPath);

    // 工具提示按钮
    HELE appButton = XBtn_Create(282 , 30, 78, 22, L"IDFonts程序", hWindow);
    XEle_EnableToolTips(appButton, true);       //启用工具提示
    XEle_SetToolTips(appButton, L"选择IDFonts.exe程序位置"); //设置工具提示内容

    HELE docButton = XBtn_Create(282, 60, 78, 22, L"选Adobe文件", hWindow);
    XEle_EnableToolTips(docButton, true);
    XEle_SetToolTips(docButton, L"选择一个Adobe文档文件");

    HELE fontButton = XBtn_Create(282, 90, 78, 22, L"字体打包目录", hWindow);
    XEle_EnableToolTips(fontButton, true);
    XEle_SetToolTips(fontButton, L"选择字体打包保存的目录");

    HELE runButton = XBtn_Create(180, 205, 90, 32, L"开始执行", hWindow);
    XEle_EnableToolTips(runButton, true);
    XEle_SetToolTips(runButton, L"开始执行字体打包任务");
    HELE closeButton = XBtn_Create(280, 205, 78, 32, L"保存关闭", hWindow);

    // 注册按钮事件
    XEle_RegisterEvent(appButton, XE_BNCLICK, appBtnClick);
    XEle_RegisterEvent(docButton, XE_BNCLICK, docBtnClick);
    XEle_RegisterEvent(fontButton, XE_BNCLICK, fontBtnClick);
    XEle_RegisterEvent(runButton, XE_BNCLICK, runBtnClick);
    XEle_RegisterEvent(closeButton, XE_BNCLICK, closeBtnClick);

}


// 事件响应
bool CALLBACK appBtnClick(HELE hEle, HELE hEventEle)
{
    char buf[MAX_PATH];
    if (GetFilePath(NULL , buf))
        charToWCHAR(appFile, buf);
    XEdit_SetText(appEdit, appFile);
    return true;
}

bool CALLBACK docBtnClick(HELE hEle, HELE hEventEle)
{
    char buf[MAX_PATH];
    if (GetFilePath(NULL , buf))
        charToWCHAR(docFile, buf);
    XEdit_SetText(docEdit, docFile);
    return true;
}

bool CALLBACK fontBtnClick(HELE hEle, HELE hEventEle)
{
    char buf[MAX_PATH];
    if (GetPath(NULL , buf))
        charToWCHAR(fontPath, buf);
    XEdit_SetText(fontEdit, fontPath);
    return true;
}

bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle)
{
     // 文本框回写配置
    XEdit_GetText(appEdit, appFile, MAX_PATH);
    XEdit_GetText(docEdit, docFile, MAX_PATH);
    XEdit_GetText(fontEdit, fontPath, MAX_PATH);

    char cmdline[2 * MAX_PATH] = {0};
    char buf[MAX_PATH] = {0};
    char* ps = WCHARTochar(buf, appFile);
    strcat(cmdline, "\"");
    strcat(cmdline, ps);
    strcat(cmdline, "\" ");

    ps = WCHARTochar(buf, docFile);
    strcat(cmdline, "\"");
    strcat(cmdline, ps);
    strcat(cmdline, "\" ");

    ps = WCHARTochar(buf, fontPath);
    strcat(cmdline, "\"");
    strcat(cmdline, ps);
    strcat(cmdline, "\" ");

  // MessageBoxA(NULL, cmdline, "Debug", MB_OKCANCEL);

    execute_command(cmdline);

    ShellExecute(NULL,"open","使用字体报告.txt",NULL,NULL,SW_SHOW);

    return true;
}

bool CALLBACK closeBtnClick(HELE hEle, HELE hEventEle)
{
    // 文本框回写配置
    XEdit_GetText(appEdit, appFile, MAX_PATH);
    XEdit_GetText(docEdit, docFile, MAX_PATH);
    XEdit_GetText(fontEdit, fontPath, MAX_PATH);

    SaveConfigFile();
    ExitProcess(0);
    return true;
}


// 执行命令行
int execute_command(char* cmdline)
{
    SetConsoleTitle(cmdline);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // 后台隐藏
    si.dwFlags   =   STARTF_USESHOWWINDOW;
    si.wShowWindow   =   SW_HIDE;

    ZeroMemory(&pi, sizeof(pi));
    // Start the child process.
    CreateProcess(NULL, TEXT(cmdline), NULL, NULL, FALSE, 0,
                  NULL, NULL, &si, &pi);
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);
    // Get the return value of the child process
    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);
    if (!ret) {
        ::MessageBox(NULL, "检测打包Adobe文档字体工具执行完成!",
                     "(C) 版权所有 2012.05 Hongwenjun (蘭公子)", MB_OKCANCEL);
    }
    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return ret;
}

// 选择一个目录
int   GetPath(HWND hWnd, char* pBuffer)
{
    BROWSEINFO   bf;
    LPITEMIDLIST   lpitem;
    memset(&bf, 0, sizeof(BROWSEINFO));
    bf.hwndOwner = hWnd;
    bf.lpszTitle = "选择路径 ";
    bf.ulFlags = BIF_RETURNONLYFSDIRS;   //属性你可自己选择
    lpitem = SHBrowseForFolder(&bf);
    if (lpitem == NULL)  //如果没有选择路径则返回   0
        return   0;

    //如果选择了路径则复制路径,返回路径长度

    SHGetPathFromIDList(lpitem, pBuffer);
    return   lstrlen(pBuffer);
}


// 选择一个文件
int   GetFilePath(HWND hWnd, char* szFile)
{

    OPENFILENAME ofn;       // common dialog box structure
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 260; // 本来sizeof(szFile);
    ofn.lpstrFilter = "Adobe文档(*.indd;*.psd;*.ai;*.pdf)\0*.indd;*.psd;*.ai;*.pdf\0IDFonts程序文件\0*.exe\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    GetOpenFileName(&ofn);
    return   lstrlen(szFile);
}

// 路径转宽字节
WCHAR* charToWCHAR(WCHAR* wch, char* czs)
{
    MultiByteToWideChar(CP_ACP, 0, czs, -1, wch, MAX_PATH); // czs 转换到宽字节wch
    return wch;
}
char* WCHARTochar(char* czs , WCHAR* wch)
{
    WideCharToMultiByte(CP_ACP, 0, wch, -1, czs, MAX_PATH , NULL, NULL);
    return czs;
}
