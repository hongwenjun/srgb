/* strtol 示例，转换一行数字 */
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char szNumbers[] = "2001 60c0c0 -1101110100110100100000 0x6fffff";
    char* pEnd; // 下一个数字的指针
    long int li1, li2, li3, li4;
    li1 = strtol(szNumbers, &pEnd, 10); // 获得第一个10进制数2001，pEnd 指向下一个数
    li2 = strtol(pEnd, &pEnd, 16); // 从 pEnd 指针开始，获得16进制数60c0c0, pEnd 指向下一个数
    li3 = strtol(pEnd, &pEnd, 2); // 进制方式: 二进制数
    li4 = strtol(pEnd, NULL, 0);  // 进制方式: 自动选择
    // 输出获取的数值
    printf("The decimal equivalents are: %ld, %lX, %ld and %lX.\n", li1, li2, li3, li4);
    return 0;
}

#if(0)
long strtol(const char* start, char** end, int base);
功能：函数返回带符号的字符串start所表示的长整型数。
参数base代表采用的进制方式。指针end指向start所表示的整型数之后的部分。
如果返回值无法用长整型表示，函数则返回LONG_MAX或LONG_MIN. 错误发生时，返回零。
#endif
