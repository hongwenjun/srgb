#include <stdio.h>
#include <string.h>
/* Find the last occurrence of C in S.
   找到C的最后一次出现在S              */
char* my_strrchr(const char* s, int c)
{
    register const char* found, *p;
    c = (unsigned char) c;
    /* Since strchr is fast, we use it rather than the obvious loop.
       由于strchr很快的，我们使用它而不是使用循环    */
    if (c == '\0')
        return strchr(s, '\0');  // 返回s的末尾

    found = NULL;
    while ((p = strchr(s, c)) != NULL) {
        found = p; // 从头开始找c，然后接着找下一个，直到找不到
        s = p + 1;
    }
    // 返回最好找到的指针
    return (char*) found;
}

int main()
{
    char str[] = "Find the last occurrence of C in S.";
    char* found = my_strrchr(str , 't');
    printf("找到位置:{%d} --> %s\n", found - str, found);

    const char* file = "file://D:/1myapp/CPP4/test/build_log.html";
    found = strrchr(file, '/') + 1;
    printf("文件基本名: %s" , found);

    return 0;
}

#if(0)
char* strchr(const char* str, int ch);
功能：函数返回一个指向str 中ch 首次出现的位置，当没有在str 中找ch到返回NULL。

char* strrchr(const char* str, int ch);
功能：函数返回一个指针，它指向字符ch 在字符串str末次出现的位置，如果匹配失败，返回NULL。
#endif
