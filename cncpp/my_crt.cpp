// 学习一点 C Runtime Library源代码 STRLEN.C STRNCAT.C STRCMP.C PRINTF.C
// http://blog.csdn.net/hongwenjun/article/details/6219985

#include <iostream>
#include <cstdio>
// #include <cstring>
size_t mystrlen(const char* str);
char* mystrcat(char* dst,  const char* src);
char* mystrcpy(char* dst, const char* src);
int mystrcmp(const char* src, const char* dst);
const int SIZE = 512;
using namespace std;
int main()
{
    char str[] = "0123456789ABCDEF";
    int len = mystrlen(str);
    cout << str << endl << "len: " << len << endl;
    char name[SIZE] ;
    char* title = name;
    printf("输入你的名字: ");
    scanf("%s", name);
    title = mystrcat(name, " 大帅哥");
    printf("Hello, %s\n", title);
    title = mystrcpy(name, "你也是大帅哥");
    printf("Hello, %s\n", title);
    int equal = mystrcmp(name, "你也是大帅哥");
    if (equal == 0)
        cout << "我们都是大帅哥\n";
    cout << "equal=: " << equal;
    return 0;
}
// VC6.0 的C Runtime Library源代码
// 目录 CRT/SRC/ 中的  STRLEN.C  STRNCAT.C  STRCMP.C
size_t mystrlen(const char* str)
{
    const char* eos = str;
    while (*eos++) ;
    return ((int)(eos - str - 1));
}
char* mystrcat(char* dst,  const char* src)
{
    char* cp = dst;
    while (*cp)
        cp++;                   /* find end of dst */
    while (*cp++ = *src++)  ;    /* Copy src to end of dst */
    /*  实际把这句 while( *cp++ = *src++) 拆开
        首先*cp = *src 赋值，然后判断表达式的值while(*cp)
        然后自增cp++, src++ 所以，整个表达式的结束判断就在于while(*cp)
        如果*cp== '/0 '，因为 '/0 '对应的ascii码就是0，所以判断就为假，也就退出循环了*/
    return (dst);                   /* return dst */
}
char* mystrcpy(char* dst, const char* src)
{
    char* cp = dst;
    while (*cp++ = *src++)  ;  /* Copy src over dst  参考上面mystrcat */
    return (dst);
}
int mystrcmp(const char* src, const char* dst)
{
    int ret = 0 ;
    // 比较 *src - *dst ,如果相等就 ret=0 ;
    // !ret && *dst ,循环结束条件(*src不等于*dst)，*dst 结束标志"/0'
    while (!(ret = *(unsigned char*)src - * (unsigned char*)dst) && *dst)
        ++src, ++dst;
    if (ret < 0)
        ret = -1 ;
    else if (ret > 0)
        ret = 1 ;
    return (ret);  //
}
#define DOT_NOT_RUN //保护其中代码不编译
#ifndef DOT_NOT_RUN
//详细解释见 http://blog.csdn.net/whs1980/archive/2004/11/13/180255.aspx
int myprintf(const char* format, ...)    /* tdout 'PRINT', 'F'ormatted */
{
    va_list arglist;  // typedef char *  va_list; 关键字typedef 允许你从一个现有的类型中创建一个新类型
    int buffing;   //缓冲区
    int retval;   //返回值
    va_start(arglist, format);   //#define _crt_va_start(ap,v)  ( ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
    _ASSERTE(format != NULL);    //#define _ASSERTE(expr) ((void)0)  //断言宏。如果输出格式字符串指针为空，则在DEBUG版下断言，报告错误。
    _lock_str2(1, stdout);       //锁定 stdout
    buffing = _stbuf(stdout);    //stdout:指定输出到屏幕
    retval = _output(stdout, format, arglist); //在output函数中，读取格式字符串中的每一个字符，然后对其进行处理，处理方式根据每一个字符所代表的意义来进行，
    //如：普通字符直接利用函数WRITE_CHAR(ch, &charsout);输出到控制台。
    _ftbuf(buffing, stdout);     // void __cdecl _ftbuf(int, FILE *);
    _unlock_str2(1, stdout);     //解锁 stdout
    return (retval);
}
#endif /* DOT_NOT_RUN */
