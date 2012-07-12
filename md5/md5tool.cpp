#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string.h>
using namespace std;

typedef struct {
    ULONG i[2];
    ULONG buf[4];
    unsigned char in[64];
    unsigned char digest[16];
} MD5_CTX;

/* MD5Init是一个初始化函数，初始化核心变量，装入标准的幻数
 * MD5Update是MD5的主计算过程，inbuf是要变换的字节串，inputlen是长度，这个函数由getMD5ofStr调用，调用之前需要调用md5init
 * MD5Final整理和填写输出结果
 */

typedef void (CALLBACK* MD5Init_Tpye)(MD5_CTX* context);
typedef void (CALLBACK* MD5Update_Tpye)(MD5_CTX* context,
                                        unsigned char* input,
                                        unsigned int inlen);
typedef void (CALLBACK* MD5Final_Tpye)(MD5_CTX* context);

char* MD5(char* pszSrc, int nSrcLength, char* pszDest)
{
    HINSTANCE hDLL = LoadLibrary("Cryptdll.dll");
    if (hDLL == NULL) {
        FreeLibrary(hDLL);
        return NULL;
    }

    MD5Init_Tpye   MD5Init;
    MD5Update_Tpye MD5Update;
    MD5Final_Tpye  MD5Final;

    MD5Init = (MD5Init_Tpye)GetProcAddress(hDLL, "MD5Init");
    MD5Update = (MD5Update_Tpye)GetProcAddress(hDLL, "MD5Update");
    MD5Final = (MD5Final_Tpye)GetProcAddress(hDLL, "MD5Final");
    if (MD5Init == NULL || MD5Update == NULL || MD5Final == NULL) {
        FreeLibrary(hDLL);
        return NULL;
    }

    MD5_CTX md5_context;
    MD5Init(&md5_context);

    MD5Update(&md5_context, (unsigned char*)pszSrc, nSrcLength);
    MD5Final(&md5_context);

    char* p = pszDest;
    for (int i = 0; i < 16; ++i) {
        sprintf(p, "%02x", md5_context.digest[i]);
        p += 2;
    }

//   cout << pszDest << endl;
    FreeLibrary(hDLL);
    return pszDest;
}

char* MD5(char* pszSrc, char* pszDest)
{
    return MD5(pszSrc , strlen(pszSrc) , pszDest);
}

static char md5_buf[65];
char* MD5(char* pszSrc)
{
    return MD5(pszSrc , strlen(pszSrc) , md5_buf);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Args Count Err! 参数错误!" << endl;

        const char* pargv = strrchr(argv[0] , '\\');
        cerr << "请使用: " << (pargv ? pargv + 1 : argv[0]) << "\t<字符串>\n";
        return -1;
    }

    char md5Dest[100];

    cout <<  argv[1] << "  --> " << MD5(argv[1] , strlen(argv[1]) , md5Dest) << endl;

    cout <<  "hello world!" << "  --> " << MD5("hello world!", md5Dest) << endl;
    cout <<  "hello world!" << "  --> " << MD5("hello world!") << endl;


    return 0;
}
