#include<stdio.h>
#include<string.h>
#include<ctype.h>
char* delAdjacentRepeat(char* s)
{
    char* ret = s; // 做为返回指针
    char* end = s; // 修改后的结束

    while (*end = *s++) {   //  复制后，使两个指针相邻
        while (*end == *s)  // 如果连续两个字符重复，源指针加1抛弃一个字符
            s++;
        end++;
    }
    *end = '\0'; // 封闭目标字串
    return ret;
}

char* delSpace(char* s)
{

    char* ret = s; // 做为返回指针
    char* end = s; // 修改后的结束

    while (isspace(*s) && *s > 0)// 删除最前面空格空格
        s++;

    while (*end = *s++) {   //  复制后，使两个指针相邻
        while (isspace(*s) && *s > 0) // 如果是空格，并且不是中文(*s > 0)
            s++;
        end++;
    }
    *end = '\0'; // 封闭目标字串
    return ret;
}


int main()
{
    const char* str = "aaaaaaabbbbbbbccccccddassss111111";
    char output[100];
    strcpy(output, str);
    printf("%s\n", delAdjacentRepeat(output));

    char cs[] = "         中文    bbbbbb    \nX\n     汉字 测试          ";
    printf("%s\n", cs);
    printf("%s\n", delSpace(cs));
    printf("%d\n", strlen(cs));

}
