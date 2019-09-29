---
title: PSD大文件垃圾数据清理工具
date: 2018-8-3
tags:  [cpp]
---

蘭雅sRGB 龙芯小本服务器 ?http://sRGB.vicp.net
	
https://github.com/hongwenjun/psdda

### PSD大文件垃圾数据清理工具

双击 PSD大文件修复工具.bat 打开一个命令行窗口

按键盘 ps 再按 TAB箭 psdda 命令会自动补全 再按下空格 ， 从文件管理器 把 psd 文件 拉到窗口里

按回车运行工具，会把修复的文件保存成 Fix_文件名.psd

再用PS 打开 Fix_psd.psd 另存一下，就能缩小文件了

由于PS 本身的BUG，智能对象中垃圾数据即使删除，另存也不能减少大小 可以有两个方法解决

把智能对象合并图层
把智能对象导出，再导入应该就可以

![](https://raw.githubusercontent.com/hongwenjun/psdda/master/psdda.gif)

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_fileSize(const char* filename);
const char* GetFileBaseName(const char* szPath);
char* memfind(const char* buf, const char* tofind, size_t len);
void helpinfo(void);

int main(int argc, char* argv[])
{
    if (argc == 1) {
        helpinfo();
        return -1;
    }

    const char* filename = argv[1];
    const char* savefile = GetFileBaseName(filename);

    if (argc == 3) {
        savefile = argv[2];
    }

    FILE* psdfile = fopen(filename, "rb");
    if (psdfile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    long filesize = get_fileSize(filename);
    size_t result;


    char* buf = new char[filesize];
    result = fread(buf, 1, filesize, psdfile);


    if (filesize > 10 * 1024 * 1024)
        printf("文件大小: %d MB\t\t文件名:%s\n",  filesize / 1024 / 1024, savefile);
    else
        printf("文件大小: %d KB\t\t文件名:%s\n",  filesize / 1024 + 1, savefile);

    fclose(psdfile);

    const char* flag_beg = "<photoshop:DocumentAncestors>" ;
    const char* flag_end = "</photoshop:DocumentAncestors>" ;

    char* pch = NULL;
    char* pch2 = NULL;
    size_t pos = 0;
    size_t pos2 = 0;
    bool flag_found = false;

    pch = memfind(buf, flag_beg, result);
    pch2 = memfind(buf, flag_end, result);

    while ((pch != NULL) && (pch2 != NULL)) {

        pos = pch - buf;
        pos2 = pch2 - buf + strlen(flag_end);

        if (pos2 < pos) {
            fputs("File error", stderr);
            exit(1);
        }

        printf("DocumentAncestors标记起止: 0x%X  -->  0x%X\n",  pos, pos2);

        memset(pch, ' ', pos2 - pos);   // 清除垃圾数据，使用空格填充

        flag_found = true;

        pch = memfind(buf + pos2, flag_beg, result - pos2);
        pch2 = memfind(buf + pos2, flag_end, result - pos2);
    }

    if (!flag_found) {
        fputs("PSD文件没有 DocumentAncestors标记，不需要工具 ", stderr);
        exit(1);
    }

    // 默认修复文件名，前缀 Fix_
    char fix_name[256];
    if (argc == 2) {
        strcpy(fix_name, "Fix_");
        strcat(fix_name, savefile);
        savefile = fix_name;
    }

    FILE* pFile = fopen(savefile, "wb");
    fwrite(buf, sizeof(char), result, pFile);

    printf("清理垃圾后文件名:\t%s\n记得要用PS打开保存才能把文件变小!\n", savefile);

    delete [] buf;
    fclose(pFile);


    return 0;
}




void helpinfo(void)
{
    printf("本工具清除PSD文件中垃圾数据 by 蘭公子 sRGB.vicp.net\n");
    printf("开源代码    https://github.com/hongwenjun/psdda\n\n");
    printf("Usage: psdda.exe  test.psd  [fix_psd.psd] \n\n");
    printf("可以 只一个文件名参数,自动加前缀Fix_; 如果2个文件名相同,覆盖文件!\n\n");
}


// 获得文件大小
size_t get_fileSize(const char* filename)
{
    FILE* pfile = fopen(filename, "rb");
    fseek(pfile, 0, SEEK_END);
    size_t size = ftell(pfile);
    fclose(pfile);
    return size;

}


// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath)
{
    const char* ret = szPath + strlen(szPath);
    while (!((*ret == '\\') || (*ret == '/'))
            && (ret != (szPath - 1))) // 得到文件名
        ret--;
    ret++;
    return ret;
}

// 内存查找字符 memfind
char* memfind(const char* buf, const char* tofind, size_t len)
{
    size_t findlen = strlen(tofind);
    if (findlen > len) {
        return ((char*)NULL);
    }
    if (len < 1) {
        return ((char*)buf);
    }

    {
        const char* bufend = &buf[len - findlen + 1];
        const char* c = buf;
        for (; c < bufend; c++) {
            if (*c == *tofind) { // first letter matches
                if (!memcmp(c + 1, tofind + 1, findlen - 1)) { // found
                    return ((char*)c);
                }
            }
        }
    }

    return ((char*)NULL);
}

```

### 命令行版本下载 

[psdda可执行文件下载](https://raw.githubusercontent.com/hongwenjun/psdda/master/psdda.exe)

### 为了适合菜鸟使用，又做了个简单图形界面

https://github.com/hongwenjun/cmd_gui





---
title: 图形界面_PSD大文件垃圾数据清理工具
date: 2018-8-3
tags:  [cpp]
---

蘭雅sRGB 龙芯小本服务器 ?http://sRGB.vicp.net
	
https://github.com/hongwenjun/cmd_gui

### 图形界面_PSD大文件垃圾数据清理工具

使用方法见图

![](/webp/guida.webp)

### 源代码片段

窗口程序就是接收程序名和参数，组合成一行命令行
然后去隐藏调用

```cpp
bool CALLBACK runBtnClick(HELE hEle, HELE hEventEle)
{

    // 文本框回写配置
    edit_text();

    // 格式化命令行
    wchar_t wbuf [2 * MAX_PATH] = {0};
    char cmdline[2 * MAX_PATH] = {0};
    swprintf(wbuf, L"\"%s\" \"%s\"  %s\\" , appFile, docFile, fontPath);
    WCHARTochar(cmdline, wbuf);

    MessageBoxA(NULL, cmdline, "注意: 确认目录存在,不能有空格", MB_OKCANCEL);
    execute_command(cmdline);


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
        MessageBoxA(NULL, "PSD大文件垃圾清理工具执行完成!",
                    "(C) 版权所有 2018.08 Hongwenjun (蘭公子)", MB_OKCANCEL);
    }
    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return ret;
}

```

