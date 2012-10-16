#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

vector<string>& sortVecData(vector<string>& vecData)  // 排序和删除重复 数字序列
{
    sort(vecData.begin(), vecData.end());  //数据排序
    vector<string>::iterator end_unique = unique(vecData.begin(), vecData.end());  // 移动重复到最后
    vecData.erase(end_unique, vecData.end());  //删除重复
    return vecData;
}


int main()
{
    // 解释参考上面函数
    char str[] = "jklmfghiqnopqaabbcdderstz";

    sort(str, str + strlen(str));
    cout << str << endl;

    char* end_unique = unique(str, str + strlen(str));
    cout << str << endl;

    *end_unique = '\0';
    cout << str << endl;

    return 0;
}