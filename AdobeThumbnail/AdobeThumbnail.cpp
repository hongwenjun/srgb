#include "AdobeThumbnail.h"
#include "atpch.h"
#include <regex>
#include <iterator>

#define MBsize 1024*1024

bool AdobeThumbnail(const char* adobe_filename , const char* savejpeg_filename)
{
    using namespace std;


    string file_ext(adobe_filename);
    string rs = "(.+)(\.(?:ai|AI|indd|INDD|Indd|eps|EPS|Eps))";  // 正则字符串，exp开始的单词
    regex expression(rs);                   // 字符串传递给构造函数，建立正则表达式
    bool ret = regex_match(file_ext, expression);
    if (!ret) {
        //      cout << "文件格式不对!\n";
        return ret ;
    }

    if (!IsFileExist(adobe_filename))
        return false ;     // 文件不存在


    char* pch = NULL;
    const char* flag = "xmpGImg:image";  // AI 和 Indd 稍微不同

    /// ************* 获取 ID或者AI文档 的预览图 **************** ///
    FILE* adobe_file = fopen(adobe_filename, "rb");
    size_t file_size = get_fileSize(adobe_filename); // 获得文件大小

    size_t bufsize = 2 * MBsize;        // AI 和EPS 预览图在开头，INDD文件在末位
    char* filebuf = new char[bufsize];  // 文件读到缓冲

    //  文件小于2M 整个文件读，否则遍历
    if (file_size < bufsize) {
        fread(filebuf, 1, file_size, adobe_file);
        pch = memfind(filebuf, flag , file_size);
        if (pch == NULL)
            ret = false ;

    } else {
        fread(filebuf, 1, bufsize, adobe_file);
//        00000000h: 06 06 ED F5 D8 1D 46 E5 BD 31 EF E7 FE 74 B7 1D ; ..眭?F褰1镧?
//        00000010h: 44 4F 43 55 4D 45 4E 54 01 70 0F 00 00 05 00 00 ; DOCUMENT.p......
        if (0xF5ED0606 == *(DWORD*)filebuf) { // indd 文件开头好像都这样
            fseek(adobe_file, bufsize, SEEK_END);
            fread(filebuf, 1, bufsize, adobe_file);
//           cout << "indd 文件格式!\n";
        }
        pch = memfind(filebuf, flag , bufsize);
        if (pch == NULL)
            ret = false;
    }

    fclose(adobe_file);

    if (!ret) {  // 没有找到，返回前
        delete[] filebuf; // 释放文件缓冲
        return ret;
    }


    strtok(pch, "\r\n");
    string Base64_str(pch);

    regex ex("xmpGImg:image\\>|<\\/xmpGImg:image\\>|xmpGImg:image=\"");
    regex en("&#xA;");
    // 正则删除 xmpGImg 标记和 转意换行替换回来
    Base64_str = regex_replace(Base64_str, ex, string(""));
    Base64_str = regex_replace(Base64_str, en, string("\n"));

    //  cout << adobe_filename << "  xmpGImg:image标记偏移: " << pch - filebuf << endl;
/// =============================== 测试解码一个Base64 的JPEG文件 ==============================////

    int b64len = Base64_str.size();
    int jpglen = fromBase64_Decode(Base64_str.c_str() , b64len , filebuf , b64len);

    FILE* jpeg_file = fopen(savejpeg_filename, "wb");
    fwrite(filebuf, 1 , jpglen , jpeg_file);


    delete[] filebuf; // 释放文件缓冲

    fclose(jpeg_file);

    return true;
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
