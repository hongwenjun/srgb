#include <iostream>
#include "ZLibWrapLib.h"
#include "ZLib/zip.h"
#include "ZLib/unzip.h"

BOOL ZipExtractCurrentFile(unzFile uf, LPCTSTR lpszDestFolder);


#define CASESENSITIVITY (0)
BOOL extract_onefile(unzFile uf, const char* filename)
{
    int err = UNZ_OK;
    if (unzLocateFile(uf, filename, CASESENSITIVITY) != UNZ_OK) {
        printf("file %s not found in the zipfile\n", filename);
        return FALSE;
    }

    printf("file %s 能够找到 in the zipfile\n", filename);

    if (!ZipExtractCurrentFile(uf, "./Test/")) {
        return FALSE;
    }

    return TRUE;
}


int main()
{
    using namespace std;

    bool ret = ZipCompress("main.cpp", "Test/a.zip", false);

    if (ret)
        cout << "压缩成功" << endl;

      ret = ZipExtract("Test/图形1.cdr", "Test/xxx");

    if (ret)
        cout << "释放成功" << endl;


    // 解压先使用 zipOpen64 来打开一个 ZIP 文件

    unzFile uf = unzOpen64("Test/图形1.cdr");

    // 需要先使用 unzGetGlobalInfo64 来取得该文件的一些信息，来了解这个压缩包里一共包含了多少个文件，等等。
    unz_global_info64 gi;

    if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
        return FALSE;
    }

    cout << gi.number_entry << " : " <<gi.size_comment << endl;

    extract_onefile(uf, "metadata/thumbnails/thumbnail.bmp");


    return 0;
}

