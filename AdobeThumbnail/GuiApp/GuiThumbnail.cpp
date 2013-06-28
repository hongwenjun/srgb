#include "GuiThumbnail.h"
#include <string.h>
#include <wchar.h>

int main(int argc , char* argv[])
{
    wchar_t src_filename[MAX_PATH] = {0};
    wchar_t thumb_filename[MAX_PATH] = {0};
    wchar_t key_word[MAX_PATH] = {0};
    wchar_t wbuf[MAX_PATH] = {0};

    for (int i = 1 ; i != argc; i++) {
        wcscat(key_word , charToWCHAR(wbuf, argv[i]));
        wcscat(key_word , L" ");
    }

    // Set the search string to abc
    Everything_SetSearch(key_word);

    // Execute the query.
    bool EQRet =  Everything_Query(TRUE);
    if (EQRet == false) {
        printf("\a全盘搜索模式需要调用软件: Everything\n\n");
        return 88;
    }

    // 文件表写文件
    FILE* pFile = fopen("filelist.txt" , "wb");
    wchar_t UNICODE_HEAD = 0xFEFF;
    fputwc(UNICODE_HEAD , pFile);

    // Display results.
    for (int i = 0 ; i < Everything_GetNumResults(); i++) {

        //    wprintf(L"%s [%s]\n", Everything_GetResultFileName(i), Everything_GetResultPath(i));

        Everything_GetResultFullPathNameW(i, src_filename , MAX_PATH);
        fwprintf(pFile, L"%s\n", src_filename);

        // AI EPS INDD 文件导出缩略图
        wcscpy(thumb_filename, Everything_GetResultFileName(i));
        wcscat(thumb_filename, L"_T.jpg");
        bool ret = AdobeThumbnail_W(src_filename , thumb_filename);

        if (!ret) { // CorelDRAW CDR 文件导出缩略图

            wcscpy(thumb_filename, Everything_GetResultFileName(i));
            wcscat(thumb_filename, L"_T.png");
            ret = CorelThumbnail_W(src_filename, thumb_filename);
        }
        if (ret)  putchar('.');

    }

    fclose(pFile);
    return 0;
}
