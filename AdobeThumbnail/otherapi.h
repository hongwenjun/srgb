// otherapi.h

#ifndef OTHERAPI_H_INCLUDED_COREL_THUMBNAIL
#define OTHERAPI_H_INCLUDED_COREL_THUMBNAIL
#include <windows.h>
#include <stdio.h>


// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName);

// 功能 获得当前路径
char* GetAppDir(char* szPath);

// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath);


// 获得文件大小
size_t get_fileSize(const char* filename);

// 内存匹配函数memfind
char* memfind(const char* buf, const char* tofind, size_t len);




#endif // OTHERAPI_H_INCLUDED_COREL_THUMBNAIL
