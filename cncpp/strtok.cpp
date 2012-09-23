#include <iostream>
#include <stdio.h>
#include <string.h>
const int SIZE = 1024 * 8;
char APPBUF[SIZE]; // 设置缓冲区 APPBUF
// 本函数完成从 分词里找最长的分词，使用缓冲区，避免了内存泄漏
char* search(const char* chbuf , const char* val);


int main()
{
    const char* str = "我/爱/中珅/||我/爱珅/中文/||我爱1234567890/中/文/||我爱/中文/分/||我爱/中文/分词/";
    char* pch = search(str , "|");
    printf("%s\n", pch);
    printf("%s\n", search("http://topic.csdn.net/u/20110822/20/e369cc10-5392-4763-b2cd-da4995b2cafb.html" , "/"));
    printf("%s.png\n", search("http://hi.csdn.net/attachment/201108/22/3362235_1313993745S1YP.png" , "/."));
    return 0;
}


char* search(const char* chbuf , const char* val)
{
    char* ret = APPBUF;  // 使用缓冲区，先清0
    memset(ret, 0, SIZE);
    char* pch = NULL;
    char* funbuf = new char[strlen(chbuf) + 1];
    strcpy(funbuf, chbuf);


    pch = strtok(funbuf, val);
    while (pch != NULL) {
        if (strlen(ret) < strlen(pch)) { // 获取最长的字符串单元
            strcpy(ret, pch);
        }
        pch = strtok(NULL, val);
    }
    delete[] funbuf;
    return ret;
}






#if 0  // 以下为代码资料，不编译


char* strtok(char* str1, const char* str2);
功能：函数返回字符串str1中紧接“标记”的部分的指针, 字符串str2是作为标记的分隔符。
如果分隔标记没有找到，函数返回NULL。为了将字符串转换成标记，
第一次调用str1 指向作为标记的分隔符。之后所以的调用str1 都应为NULL。


char* strtok(char* string, const char* control)
{
    unsigned char* str;
    const unsigned char* ctrl = control;


    unsigned char map[32];
    int count;


    static char* nextoken;


    /* Clear control map */
    for (count = 0; count < 32; count++)
        map[count] = 0;


    /* Set bits in delimiter table */
    do {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);


    /* Initialize str. If string is NULL, set str to the saved
     * pointer (i.e., continue breaking tokens out of the string
     * from the last strtok call) */
    if (string)
        str = string;
    else
        str = nextoken;


    /* Find beginning of token (skip over leading delimiters). Note that
     * there is no token iff this loop sets str to point to the terminal
     * null (*str == '\0') */
    while ((map[*str >> 3] & (1 << (*str & 7))) && *str)
        str++;


    string = str;


    /* Find the end of the token. If it is not the end of the string,
     * put a null there. */
    for (; *str ; str++)
        if (map[*str >> 3] & (1 << (*str & 7))) {
            *str++ = '\0';
            break;
        }


    nextoken = str;


    /* Determine if a token has been found. */
    if (string == str)
        return NULL;
    else
        return string;
}

#endif
