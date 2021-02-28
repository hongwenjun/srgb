#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// strtrim 去掉字符串前后的空格和制表符
char* web_strtrim(char* s)
{
    char* p = s;
    char* q = s;

    while (*p == ' ' || *p == '\t')
        ++p;

    while (*q++ = *p++)
        ;
    q -= 2;

    while (*q == ' ' || *q == '\t')
        --q;
    *(q + 1) = '\0';

    return s;
}

char* x_strtrim(char* s)
{
    char* p = s;
    while (isspace(*p))
        ++p;
    strcpy(s, p);

    strrev(s);
    p = s;

    while (isspace(*p))
        ++p;

    strcpy(s, p);
    strrev(s);

    return s;
}

char* strtrim(char* s)
{
    char* p = s;
    while (isspace(*p))
        ++p;

    char* end = s + strlen(s) - 1;
    while (isspace(*end))
        --end;
    *(end + 1) = '\0';

    strcpy(s, p);
    return s;
}

int main()
{
    char s[] = "   你好世界 \n ";

    char* str = strtrim(s);
    printf("%s<<----<<<<\n", s);
    return 0;
}
