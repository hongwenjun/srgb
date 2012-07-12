#include <windows.h>
#include <iostream>
#include <stdio.h>
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

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Args Count Err! 参数错误!" << endl;

        const char* pargv = strrchr(argv[0] , '\\');
        cerr << "请使用: " << (pargv ? pargv + 1 : argv[0]) << "\t<字符串>\n";
        return 0;
    }

    HINSTANCE hDLL = LoadLibrary("Cryptdll.dll");
    if (hDLL == NULL) {
        return -1;
    }

    MD5Init_Tpye   MD5Init;
    MD5Update_Tpye MD5Update;
    MD5Final_Tpye  MD5Final;

    MD5Init = (MD5Init_Tpye)GetProcAddress(hDLL, "MD5Init");
    MD5Update = (MD5Update_Tpye)GetProcAddress(hDLL, "MD5Update");
    MD5Final = (MD5Final_Tpye)GetProcAddress(hDLL, "MD5Final");
    if (MD5Init == NULL || MD5Update == NULL || MD5Final == NULL) {
        FreeLibrary(hDLL);
        return -1;
    }

    MD5_CTX md5_context;
    MD5Init(&md5_context);
    unsigned char src[100];

    unsigned length = strlen(argv[1]);
    memcpy(src, argv[1], length);


    MD5Update(&md5_context, src, length);
    MD5Final(&md5_context);
    char dest[100] = { 0 };
    char* p = dest;
    for (int i = 0; i < 16; ++i) {
        sprintf(p, "%02x", md5_context.digest[i]);
        p += 2;
    }
    cout << dest << endl;
    FreeLibrary(hDLL);
    return 0;
}
