#include <iostream>
#include <string>
#include <regex>

using namespace std;
int main()
{
    string str("This expression could match from A and beyond. [ expression  express ]");
    string rs = "exp\\w*";  // 正则字符串，exp开始的单词
    cout << str << endl;

    regex expression(rs); // 字符串传递给构造函数，建立正则表达式

// regex_match 判断一个正则表达式(参数 e)是否匹配整个字符序列 str. 它主要用于验证文本。
// 注意，这个正则表达式必须匹配被分析串的全部，否则函数返回 false.
// 如果整个序列被成功匹配，regex_match 返回 True.
    bool ret = regex_match(str, expression);
    if (ret)
        cout << "可以匹配整个文本" << endl;
    else
        cout << "不能匹配整个文本" << endl;

// regex_search 类似于 regex_match, 但它不要求整个字符序列完全匹配。
// 你可以用 regex_search 来查找输入中的一个子序列，该子序列匹配正则表达式 e.
    ret = regex_search(str, expression);
    if (ret)
        cout << "能够搜索到 " << rs << endl;
    else
        cout << "不能搜索" << endl;

// regex_replace 在整个字符序列中查找正则表达式e的所有匹配。
// 这个算法每次成功匹配后，就根据参数fmt对匹配字符串进行格式化。
// 缺省情况下，不匹配的文本不会被修改，即文本会被输出但没有改变。

    str = regex_replace(str, expression, string("表达式"));
// VC2010或者TR1注意：这里第三个参数要用string()转换
// http://www.johndcook.com/cpp_regex.html
// started with C++ TR1 regular expressions
    cout << str << endl;
    return 0;
}