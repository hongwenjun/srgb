#include <cstdio>      // 定义输入／输出函数
#include <cctype>      // 字符处理
#include <cstring>
#include <stdlib.h>


//  13进制数 转 10进制
int thir_to_base10(const char* tr);

//  10进制数 转 13进制字符
char* base10_to_thir(int x, char* tr);

int main()
{
    int cnt = 0;
    scanf("%d\n", &cnt);
    // fflush(stdin); //  因为getline会单独读入一次之前输入遗留的回车符，所以你scanf里加上\n反而是对的。
    // 后面加上fflush(stdin);清空输入缓冲区就行了

    int x = 0;
    char thir_txt[32];
    char line[260];

    for (int i = 0; i != cnt ; i++) {
        gets(line);
        if (isdigit(line[0])) {

            sscanf(line, "%d", &x);
            printf("%s\n", base10_to_thir(x, thir_txt));

        } else if (isalpha(line[0])) {
            x = thir_to_base10(line);
            printf("%d\n", x);
        }
    }
    return 0;
}

/*
    地球人的0被火星人称为tret。
    地球人数字1到12的火星文分别为：jan, feb, mar, apr, may, jun, jly, aug, sep, oct, nov, dec。
    火星人将进位以后的12个高位数字分别称为：tam, hel, maa, huh, tou, kes, hei, elo, syy, lok, mer, jou。
    例如地球人的数字“29”翻译成火星文就是“hel mar”；而火星文“elo nov”对应地球数字“115”。
    为了方便交流，请你编写程序实现地球和火星数字之间的互译。

*/


char* thir_low[13] = {"tret",                 //基数 0-13        // 0
                      "jan", "feb", "mar", "apr", "may", "jun",  // 1 2 3 4 5 6
                      "jly", "aug", "sep", "oct", "nov", "dec"   // 7 8 9 A B C
                     };

char* thir_high[13] = {"",
                       "tam", "hel", "maa", "huh", "tou", "kes", // 10 20 30 40 50 60
                       "hei", "elo", "syy", "lok", "mer", "jou"  // 70 80 90 A0 B0 C0
                      };

//  13进制字符 转 10进制数
int thir_to_base10(const char* tr)
{
    char buf[3] = "00";
    for (int h = 1 ; h != 13 ; ++h) {
        if (strstr(tr, thir_high[h]) != NULL) {
            buf[0] = h + '0';
            if (h > 9)
                buf[0] = h - 10 + 'A';
        }

    } // 分别高位和低位填充

    for (int l = 1 ; l != 13 ; ++l) {
        if (strstr(tr, thir_low[l]) != NULL) {
            buf[1] = l + '0';
            if (l > 9)
                buf[1] = l - 10 + 'A';
        }
    }

    //  puts(buf);
    char* pEnd;
    int x = strtol(buf, &pEnd, 13);

    return x;
}

//  10进制数 转 13进制字符
char* base10_to_thir(int x, char* tr)
{
    if (x > 169)
        return NULL;

    int h, l;  // 13进制高位低位
    h = x / 13;
    l = x % 13;

    if (h)
        sprintf(tr, "%s %s", thir_high[h], thir_low[l]);
    else
        sprintf(tr, "%s", thir_low[l]);

    return tr;
}
