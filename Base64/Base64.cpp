#include <iostream>
#include <fstream>
#include <sstream>
#include <string>>
#include <atlenc.h>
#include <stdio.h>.h>

// 获得编码后的数据长度
#define getDecodeLength Base64EncodeGetRequiredLength

// 数据使用 Base64 编码, 返回编码后的长度，如果返回0，可能目标空间不够
int toBase64_Encode(const char* pSrc, int nLenSrc, char* pDst, int nLenDst);

// Base64 数据解码, 返回原来数据的长度，如果返回0，可能目标空间不够
int fromBase64_Decode(const char* pSrc, int nLenSrc, char* pDst, int nLenDst);


using namespace std;

int main()
{
    /// ==============================  Base64 编码 ====================================== ///

    string  str = "Man is distinguished, not only by his reason, but by this singular passion from other"
                  " animals, which is a lust of the mind, that by a perseverance of delight in the continued"
                  " and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

    char buf[1024] = {0};
    int len = toBase64_Encode(str.c_str() , str.size() , buf , 1024);

    cout << str << str.size() << endl << buf << endl << len << endl;

    /// ==============================  Base64 解码 ====================================== ///

    string  strb64 = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGl\n"
                     "zIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2\n"
                     "YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29ud\n"
                     "GludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRz\n"
                     "IHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=\n";
    len = fromBase64_Decode(strb64.c_str() , strb64.size() , buf , 1024);
    buf[len] ='\0';

    cout << buf << endl;

    /// =============================== 测试解码一个Base64 的JPEG文件 ==============================////

    ifstream  infile("Base64.txt");
    stringstream  oss;
    oss << infile.rdbuf();
    str = oss.str();
    len = str.size();
    char* filebuf = new char[len + 1];

    len = fromBase64_Decode(str.c_str() , len , filebuf , len);

    ofstream  outfile("test.jpg" , ios_base::binary);
    outfile.write(filebuf, len);
    cout << "\nBase64.txt   ---> 解码成 test.jpg\n";
    infile.close();
    outfile.close();
    delete[] filebuf;

    /// =============================== 测试JPEG文件Base64 编码==============================////
    FILE* pFile = fopen("test.jpg" , "rb");

    fseek(pFile, 0 , SEEK_END);
    int size = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    char* bufSrc = new char[size + 1];
    char* bufDst = new char[getDecodeLength(size) + 1];

    fread(bufSrc, 1, size, pFile);

    len = toBase64_Encode(bufSrc , size, bufDst , getDecodeLength(size) + 1);

    cout << len << size << endl;
    ofstream  txtfile("newBase64.txt");
    txtfile << bufDst << endl;
    cout << "\ntest.jpg   ---> 编码成 newBase64.txt\n";

    txtfile.close();
    delete[] bufDst;

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

