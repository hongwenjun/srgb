// 英语 单词计数 和分词
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    char str[] =
        "Dunkin' Donuts is America's favorite every day, \n"
        "all-day stop for coffee and baked goods. Dunkin' \n"
        "Donuts is a market leader in the regular/decaf coffee, iced.";

    char* copystr = (char*) malloc(strlen(str) + 1);
    strcpy(copystr, str);  // 使用 str的副本执行分词操作

    int cnt = 0;
    char* pch = NULL;
    pch = strtok(copystr, " \t\n.,"); // 查找间隔符号替换成 '\0'
    while (pch != NULL) {
        printf("%d\t%s\n" , ++cnt, pch);
        pch = strtok(NULL, " -/\t\n.,!");
    }

    free(copystr);
    return 0;
}
