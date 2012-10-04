#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "sqlite3.h"


typedef SQLITE_API char* (*SQLITE3_pf)(const char* zFilename);  // 定义函数指针类型
SQLITE3_pf sqlite3_win32_utf8_to_mbcs = NULL;      // 函数指针声明
SQLITE3_pf sqlite3_win32_mbcs_to_utf8 = NULL;

int main()
{
    char str[] = "SELECT语句用于查询数据库。一条SELECT命令的返回结果是零或多行每行有固定字段数的数据。"
                 "字段的数目由在SELECT和FROM之间的表达式列表定义。任意的表达式都可以被用作结果。若表达式是 "
                 "*则表示所有表的所有字段。若表达式是表的名字后接.*则结果为该表中的所有字段。";
    char utf8str[1024] = {0};

    HINSTANCE hDLL;
    if ((hDLL = LoadLibrary("sqlite3.dll")) > 0) {   // 加载动态库文件，以获得两个函数指针的地址
        sqlite3_win32_utf8_to_mbcs = (SQLITE3_pf)GetProcAddress(hDLL, "sqlite3_win32_utf8_to_mbcs");
        sqlite3_win32_mbcs_to_utf8 = (SQLITE3_pf)GetProcAddress(hDLL, "sqlite3_win32_mbcs_to_utf8");
    }

    SQLITE_API char* sp = sqlite3_win32_mbcs_to_utf8(str);   // 转成 utf-8
    printf("%s\t\t内存: %x\n" , sp, sp);

    FILE* fp = fopen("utf8.txt", "w + ");
    strcpy(utf8str , sp);
    fprintf(fp, "%s\n", utf8str);

    sqlite3_free(sp);


    SQLITE_API char* sp2 = sqlite3_win32_utf8_to_mbcs(utf8str);
    printf("%s\t\t内存: %x\n" , sp2, sp2);
    sqlite3_free(sp2);

    printf("%d    %d\n" , strlen(sp), strlen(sp2));

    return 0;
}

#if 0
/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from sqlite3_malloc().
*/
SQLITE_API char* sqlite3_win32_mbcs_to_utf8(const char* zFilename)
{
    char* zFilenameUtf8;
    LPWSTR zTmpWide;

    zTmpWide = mbcsToUnicode(zFilename);
    if (zTmpWide == 0) {
        return 0;
    }
    zFilenameUtf8 = unicodeToUtf8(zTmpWide);
    sqlite3_free(zTmpWide);
    return zFilenameUtf8;
}

/*
** Convert UTF-8 to multibyte character string.  Space to hold the
** returned string is obtained from sqlite3_malloc().
*/
SQLITE_API char* sqlite3_win32_utf8_to_mbcs(const char* zFilename)
{
    char* zFilenameMbcs;
    LPWSTR zTmpWide;

    zTmpWide = utf8ToUnicode(zFilename);
    if (zTmpWide == 0) {
        return 0;
    }
    zFilenameMbcs = unicodeToMbcs(zTmpWide);
    sqlite3_free(zTmpWide);
    return zFilenameMbcs;
}

/*
** Free memory previously obtained from sqlite3Malloc().
*/
SQLITE_API void sqlite3_free(void *p){
  if( p==0 ) return;  /* IMP: R-49053-54554 */
  assert( sqlite3MemdebugNoType(p, MEMTYPE_DB) );
  assert( sqlite3MemdebugHasType(p, MEMTYPE_HEAP) );
  if( sqlite3GlobalConfig.bMemstat ){
    sqlite3_mutex_enter(mem0.mutex);
    sqlite3StatusAdd(SQLITE_STATUS_MEMORY_USED, -sqlite3MallocSize(p));
    sqlite3StatusAdd(SQLITE_STATUS_MALLOC_COUNT, -1);
    sqlite3GlobalConfig.m.xFree(p);
    sqlite3_mutex_leave(mem0.mutex);
  }else{
    sqlite3GlobalConfig.m.xFree(p);
  }
}

#endif
