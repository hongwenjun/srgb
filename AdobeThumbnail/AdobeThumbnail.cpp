 #include "AdobeThumbnail.h"
#include "atpch.h"
#include <regex>
#include <iterator>

#define MBsize 1024*1024

int AdobeThumbnail(const char* adobe_filename , const char* savejpeg_filename)
{
    using namespace std;

    if (!IsFileExist(adobe_filename))
        return -1 ;     // 文件不存在

    char* pch = NULL;
    const char* flag = "xmpGImg:image";  // AI 和 Indd 稍微不同

    /// ************* 获取 ID或者AI文档 的预览图 **************** ///
    FILE* adobe_file = fopen(adobe_filename, "rb");
    FILE* jpeg_file = fopen(savejpeg_filename, "wb");


    size_t file_size = get_fileSize(adobe_filename); // 获得文件大小

    size_t bufsize = 4 * MBsize;
    char* filebuf = new char[bufsize];  // 文件读到缓冲

    //  文件小于4M 处理
    if (file_size < bufsize) {
        fread(filebuf, 1, file_size, adobe_file);
        pch = memfind(filebuf, flag , file_size);
        strtok(pch, "\r\n");
        if (pch != NULL)
            ;//    cout << pch << endl;
    }

    string Base64_str(pch);

    regex ex("xmpGImg:image\\>|\\<\\/xmpGImg:image\\>|xmpGImg:image=\"");
    regex en("&#xA;");
    // 正则删除 xmpGImg 标记和 转意换行替换回来
    Base64_str = regex_replace(Base64_str, ex, string(""));
    Base64_str = regex_replace(Base64_str, en, string("\n"));

/// =============================== 测试解码一个Base64 的JPEG文件 ==============================////

    int b64len = Base64_str.size();
    int jpglen = fromBase64_Decode(Base64_str.c_str() , b64len , filebuf , b64len);
    fwrite(filebuf, 1 , jpglen , jpeg_file);


    delete[] filebuf; // 释放文件缓冲
    fclose(adobe_file);
    fclose(jpeg_file);

    return 0;
}




int toBase64_Encode(const char* pSrc, int nLenSrc, char* pDst, int nLenDst)
{
    int nDestLen = nLenDst;
    BOOL fRet = Base64Encode((BYTE*) pSrc, nLenSrc, pDst, &nDestLen,  ATL_BASE64_FLAG_NONE); // 可以选择编码是否分行
    if (!fRet) nDestLen = 0;
    return (nDestLen);
}

int fromBase64_Decode(const char* pSrc, int nLenSrc, char* pDst, int nLenDst)
{
    int nDestLen = nLenDst;
    BOOL fRet = Base64Decode((LPCSTR)pSrc, nLenSrc, (BYTE*)pDst, &nDestLen);
    if (!fRet) nDestLen = 0;
    return (nDestLen);
}

