#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;
int getDigital(const string& str)
{
    int len = str.size();
    const char* ps = str.c_str();
    while (!isdigit(*ps++) && --len)    // 跳过开头不是数字
        ;
    --ps;                       // 补偿ps++ ，跳回到数字头

    if (*(ps - 1) == '-')        // 检测整数符号
        --ps;
    return atoi(ps);            // 转换成数字
}

int main()
{
    cout << getDigital("中国陆地面积960万平方公里！") << endl;
    return 0;
}
