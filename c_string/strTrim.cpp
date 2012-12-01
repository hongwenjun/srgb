#include <iostream>
#include <string>
#include <cstring>

using namespace std;

// 删除字符串前后空白
string strTrim(string& str)
{
    str = str.substr(str.find_first_not_of("\t \n"));        // 查找非空白处获取子串
    str = str.substr(0, str.find_last_not_of("\t \n") + 1);  // 找最后一个非空白处获取子串
    return str;
}



// VC6.0 的_strrev 的源码 _strrev 不是标准库，所以可以应急自己写一个
char* cs_strrev(char* string)
{
    char* start = string;
    char* left = string;
    char ch;

    while (*string++) // 找到string 末位的'\0'，
        ;             // 因为上面 string++，实际指针在 '\0'的后一个
    string -= 2;      // 所以退2格，回到字符串最后一个字母
    while (left < string) {
        ch = *left;    // 通过临时变量 ch 进行交换
        *left++ = *string;
        *string-- = ch;
    }
    return(start);
}

char* csTrim(char* cs)
{
    char* ret = cs;
    char* pc = new char[strlen(cs) + 1];

    int pos = strspn(cs , "\t \n");     // 查找非空白处pos
    strcpy(pc , cs + pos);
    cs_strrev(pc);                        // 反序字符串
    pos = strspn(pc , "\t \n");         // 原来后面的空白，就变成了前面的空白
    strcpy(cs , pc + pos);
    cs_strrev(cs);                        // 再反序，还原回来

    delete[] pc;
    return ret;
}

char* csTrim_v2(char* cs)
{
    char* ret = cs;
    char* pc = new char[strlen(cs) + 1];

    int pos = strspn(cs , "\t \n");     // 查找非空白处pos
    strcpy(pc , cs + pos);

    char* end = pc;
    while (*end++)     // 找到字符串末位的'\0'，
        ;              // 因为上面 end++，实际指针在 '\0'的后一个
    end -= 2;          // 所以退2格，回到字符串最后一个字母

    while (*end == ' ' || *end == '\t' ||  *end == '\n') {
        *end-- = '\0';
    }
    strcpy(cs , pc);

    delete[] pc;
    return ret;
}





int main()
{
    string str = " \t \n  你好世界!  \t \n ";
    char cs[] =  " \t \n  你好世界!  \t \n ";

    cout << strTrim(str)  << csTrim_v2(cs) << endl;
    return 0;
}
