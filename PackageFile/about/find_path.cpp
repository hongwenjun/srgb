#include <windows.h>
#include <stdio.h>

FILE* pf_path_file;

void find_path(char* lp_path)
{
    // fprintf(p, "目录%s下的文件/n", lp_path);
    HANDLE hFile;  // windows对文件的操作首先要得到一个文件句柄
    FILETIME ft;  SYSTEMTIME st; // 文件时间 和 系统时间
    WIN32_FIND_DATAA wfd;  // WIN32_FIND_DATAA 是windows定义的查找文件的结构
    char sz_path[256];
    strcpy(sz_path, lp_path);
    strcat(sz_path, "\\*.*");
    hFile = FindFirstFile(sz_path, &wfd);  // FindFirstFile函数查找一个文件，sz_path是要查找的文件名，可以是全路径或相对路径，也可以写通配符，如“c:/*.*”
    if (hFile != INVALID_HANDLE_VALUE) {
        while (FindNextFile(hFile, &wfd)) {  // 利用第一次找到的文件句柄，继续寻找下个文件，如果找到下个文件，则函数填充wfd结构，并返回true
            if (wfd.cFileName[0] == '.')continue; // . 当前目录 .. 上级目录符号  跳过去

            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { // 判断，得出找到的文件名是否是一个文件夹，如果是，则递归调用查找函数，由此实现深度全文件夹搜索
                strcpy(sz_path, lp_path);
                strcat(sz_path, "\\");
                strcat(sz_path, wfd.cFileName);
                find_path(sz_path);   // 低谷递归查找

            } else {
                ft = wfd.ftLastWriteTime;
                FileTimeToSystemTime(&ft, &st);

                printf("%d-%02d-%02d  %02d:%02d  ", st.wYear, st.wMonth, st.wDay, st.wHour + 8, st.wMinute);
                printf("%10u | %s | %s\n", wfd.nFileSizeLow , wfd.cFileName , lp_path  );

             //   fprintf(pf_path_file , "%u , %s , %s\n", wfd.nFileSizeLow ,   wfd.cFileName , lp_path);
            }
        }
    }

}
int main(int argc, char* argv[])
{
    pf_path_file = fopen("asd.txt", "w");
    char sz_path[12] = "D:\\雅2010"; // "c:" 设定默认盘符,
    find_path(sz_path);       // 搜索盘符
    fclose(pf_path_file);
    return 0;
}


#if 0
http://bbs.csdn.net/topics/340009854
收藏 获取指定目录下的文件信息...输出到txt


文件属性的获取与更改：


一、 引言
文件是数据在磁盘上最常用的一种存放形式，也是在程序设计中与之经常打交道的一种编程对象，不少程序尤其是数据传输和处理类的应用程序更是需要频繁的创建、读取和写入文件。对于一些要求不是很严格的程序，我们往往只关心文件的内容是否正确、文件大小是否有增减或是再严格一些，看文件名是否符合规定等等。以上这些要素对于大多数程序而言显然是可以满足实际需求的，但对于某些特殊行业的一些有着比较严格要求的软件系统，仅有以上要素还是远远不够的，往往还需要对文件的所有属性诸如文件的创建时间、文件的最后访问时间、文件的最后修改时间等等进行提取处理与重新设置。

　　 二、 WIN32_FIND_DATA结构

　　 关于文件的全部属性信息，总计有以下以下9种：文件的标题名、文件的属性（只读、存档，隐藏等）、文件的创建时间、文件的最后访问时间、文件的最后修改时间、文件大小的高位双字、文件大小的低位双字、保留、保留。在这里只有文件标题名和文件的长度可以通过CFile类比较方便的获得，而对于其他几种属性的获取和设置就无能为力了。

　　 在用findfirst()和findnext()函数去查找磁盘文件时经常使用的一个数据结构WIN32_FIND_DATA的成员变量里包含了以上所有的文件属性，因此可以通过这个结构作为获取和更改文件属性的手段。该结构的内容如下：

typedef struct _WIN32_FIND_DATA {
　　 DWORD dwFileAttributes; //文件属性
　　 FILETIME ftCreationTime; // 文件创建时间
　　 FILETIME ftLastAccessTime; // 文件最后一次访问时间
　　 FILETIME ftLastWriteTime; // 文件最后一次修改时间
　　 DWORD nFileSizeHigh; // 文件长度高32位
　　 DWORD nFileSizeLow; // 文件长度低32位
　　 DWORD dwReserved0; // 系统保留
　　 DWORD dwReserved1; // 系统保留
　　 TCHAR cFileName[ MAX_PATH ]; // 长文件名
　　 TCHAR cAlternateFileName[ 14 ]; // 8.3格式文件名
} WIN32_FIND_DATA, *PWIN32_FIND_DATA;

　　 可以通过FindFirstFile（）函数根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去：

WIN32_FIND_DATA ffd ;
HANDLE hFind = FindFirstFile("c:\\test.dat",&ffd);

　　 在使用这个结构时不能手工修改这个结构中的任何数据，结构对于开发人员来说只能作为一个只读数据，其所有的成员变量都会由系统完成填写。在MSDN帮助中可以查找到关于WIN32_FIND_DATA结构的更加详细的说明。

　　 三、 文件属性信息的获取与更改

　　 为了更好的保存获取到的文件属性信息，对应于文件属性构造一个自定义的FILE_INFO数据结构，获取的属性信息可暂存于此：

typedef struct _FILE_INFO {
　　 TCHAR szFileTitle[128]; //文件的标题名
　　 DWORD dwFileAttributes; //文件的属性
　　 FILETIME ftCreationTime; //文件的创建时间
　　 FILETIME ftLastAccessTime; //文件的最后访问时间
　　 FILETIME ftLastWriteTime; //文件的最后修改时间
　　 DWORD nFileSizeHigh; //文件大小的高位双字
　　 DWORD nFileSizeLow; //文件大小的低位双字
　　 DWORD dwReserved0; //保留，为0
　　 DWORD dwReserved1; //保留，为0
} FILE_INFO, * PFILE_INFO;

　　 首先用FindFirstFile（）函数将文件属性获取到WIN32_FIND_DATA 结构对象FindFileData中去，之后可以用FindClose（）将其关闭，并把FindFileData中的有关文件属性信息的内容复制到自定义结构FILE_INFO的结构对象FileInfo中备用。下面是关于这部分描述的部分关键代码：

//声明结构对象
FILE_INFO FileInfo;
WIN32_FIND_DATA FindFileData;
……
//获取文件属性信息
FindClose(FindFirstFile("Test.txt",&FindFileData));
memset(&FileInfo,0,sizeof(FILE_INFO));
……
//将文件属性信息保存到FileInfo中备用
strcpy(FileInfo.szFileTitle,myFile.GetFileTitle());
FileInfo.dwFileAttributes = FindFileData.dwFileAttributes;
FileInfo.ftCreationTime = FindFileData.ftCreationTime;
FileInfo.ftLastAccessTime = FindFileData.ftLastAccessTime;
FileInfo.ftLastWriteTime = FindFileData.ftLastWriteTime;
FileInfo.nFileSizeHigh = FindFileData.nFileSizeHigh;
FileInfo.nFileSizeLow = FindFileData.nFileSizeLow;
……

　　 在获取到文件的原始属性信息后既可以原封不动的将属性重新写到文件，也可以对其中某一项或某几项属性内容进行修改后再行写入文件，从而达到更改文件属性的目的。比如可以用SetFileTime()函数设置文件的创建时间、最近一次访问时间以及最近一次修改的时间等等：

SetFileTime((HANDLE)destFile.m_hFile, //待写入的文件句柄
&FileInfo.ftCreationTime, //文件的创建时间
&FileInfo.ftLastAccessTime, //文件最近一次的访问时间
&FileInfo.ftLastWriteTime); //文件最近一次的修改时间

　　 也可以用SetFileAttributes() 函数实现对文件属性的修改：

SetFileAttributes(FileInfo.szFileTitle,FileInfo.dwFileAttributes);

　　 至于文件名的修改则更加简单，直接在创建文件时在CreateFile()或CFile类的成员函数Open里直接对文件名参数进行设置即可。

　　 小结：本文通过对WIN32_FIND_DATA结构和SetFileTime（）、SetFileAttributes（）等主要函数实现了对磁盘文件的相关属性信息的获取与修改。用此技术可以在通讯等对文件有严格要求的应用领域实现文件全部信息（包括文件内容、文件名以及文件属性等）的完整传送。本文所述程序在Windows 98下由Microsoft Visual C++ 6.0编译调试通过。

#include "stdafx.h"
#include <windows.h>
#define FILEILTER "*.*"

BOOL IsRoot(LPCTSTR lpszPath)
{
    TCHAR szRoot[4];
    wsprintf(szRoot, "%c:\\", lpszPath[0]);
    return (lstrcmp(szRoot, lpszPath) == 0);
}

void FindInAll(LPCTSTR lpszPath)
{
    TCHAR szFind[MAX_PATH];
    lstrcpy(szFind, lpszPath);
    if (!IsRoot(szFind))
        lstrcat(szFind, "\\");
    lstrcat(szFind, FILEILTER); // 找所有文件
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(szFind, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
        return;

    do
    {
        if (wfd.cFileName[0] == '.')
            continue; // 过滤这两个目录
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            TCHAR szFile[MAX_PATH];
            if (IsRoot(lpszPath))
                wsprintf(szFile, "%s%s", lpszPath, wfd.cFileName);
            else
            {
                wsprintf(szFile, "%s\\%s", lpszPath, wfd.cFileName);
                FindInAll(szFile); // 如果找到的是目录，则进入此目录进行递归
            }
        }
        else
        {
            TCHAR szFile[MAX_PATH];
            if (IsRoot(lpszPath))
            {
                wsprintf(szFile, "%s%s", lpszPath, wfd.cFileName);
            }
            else
            {
                wsprintf(szFile, "%s\\%s", lpszPath, wfd.cFileName);
                printf("%s\n",szFile);
            }
            // 对文件进行操作
        }
    } while (FindNextFile(hFind, &wfd));
    FindClose(hFind); // 关闭查找句柄

}
int main(int argc, char* argv[])
{
    FindInAll("C:\\TEST");
    return 0;
}

把两部分的功能合成一下，然后把结果输出到 txt 就好了....


//#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#define FILEILTER "*.*"

FILE *fp;
BOOL IsRoot(LPCTSTR lpszPath)
{
    TCHAR szRoot[4];
    wsprintf(szRoot, "%c:\\", lpszPath[0]);
    return (lstrcmp(szRoot, lpszPath) == 0);
}
void PrintFileDetail(WIN32_FIND_DATA *wfd)
{
    SYSTEMTIME sysTime;

    fprintf(fp,"文件名：%s\n",wfd->cFileName);
    FileTimeToSystemTime(&(wfd->ftCreationTime),&sysTime);
    fprintf(fp,"文件的创建时间      :%d年 %d月 %d日 星期%d, %d时 %d分 %d秒\n", sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wDayOfWeek,sysTime.wHour+8,sysTime.wMinute,sysTime.wSecond);
    FileTimeToSystemTime(&(wfd->ftLastAccessTime),&sysTime);
    fprintf(fp,"文件的最后访问时间  :%d年 %d月 %d日 星期%d, %d时 %d分 %d秒\n", sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wDayOfWeek,sysTime.wHour+8,sysTime.wMinute,sysTime.wSecond);
    FileTimeToSystemTime(&(wfd->ftLastWriteTime),&sysTime);
    fprintf(fp,"文件的最后修改时间  :%d年 %d月 %d日 星期%d, %d时 %d分 %d秒\n", sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wDayOfWeek,sysTime.wHour+8,sysTime.wMinute,sysTime.wSecond);
    fprintf(fp,"文件属性：");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
    fprintf(fp,"归档 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
    fprintf(fp,"压缩文件 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    fprintf(fp,"隐藏 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    fprintf(fp,"只读 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
    fprintf(fp,"系统文件 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
    fprintf(fp,"临时文件 ");
    if(wfd->dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
    fprintf(fp,"默认 ");
    fprintf(fp,"\n");;
    fprintf(fp,"文件大小：%d字节\n",wfd->nFileSizeLow);
}

void FindInAll(LPCTSTR lpszPath)
{
    TCHAR szFind[MAX_PATH];
    lstrcpy(szFind, lpszPath);
    if (!IsRoot(szFind))
        lstrcat(szFind, "\\");
    lstrcat(szFind, FILEILTER); // 找所有文件
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(szFind, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
        return;

    do
    {
        if (wfd.cFileName[0] == '.')
            continue; // 过滤这两个目录
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            TCHAR szFile[MAX_PATH];
            if (IsRoot(lpszPath))
                wsprintf(szFile, "%s%s", lpszPath, wfd.cFileName);
            else
            {
                wsprintf(szFile, "%s\\%s", lpszPath, wfd.cFileName);
                FindInAll(szFile); // 如果找到的是目录，则进入此目录进行递归
            }
        }
        else
        {
            TCHAR szFile[MAX_PATH];
            if (IsRoot(lpszPath))
            {
                wsprintf(szFile, "%s%s", lpszPath, wfd.cFileName);
            }
            else
            {
                wsprintf(szFile, "%s\\%s", lpszPath, wfd.cFileName);
                printf("%s\n",szFile);
                PrintFileDetail(&wfd);
            }
            // 对文件进行操作
        }
    } while (FindNextFile(hFind, &wfd));
    FindClose(hFind); // 关闭查找句柄

}
int main(int argc, char* argv[])
{
    char findFile[64]={0},saveFile[64]={0};

    puts("输入文件路径: (如 d:\\\\test)");
    gets(findFile);
    puts("输入要保存结果的文件:(如 d:\\\\test.txt)");
    gets(saveFile);
    fp=fopen(saveFile,"a+");
    fprintf(fp,"目录%s 下的文件为：\n\n",findFile);
    FindInAll(findFile);
    printf("文件保存至%s.\n",saveFile);
    fclose(fp);
    getchar();
    return 0;
}







#endif
