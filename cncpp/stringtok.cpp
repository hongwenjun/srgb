#include <iostream>    // 数据流输入／输出
#include <string>      // 字符串类
#include <algorithm>   // STL 通用算法
#include <vector>

using namespace std;
// 使用value中字符，切割str字符串，存到 strtok_vec容器里
vector<string>& stringtok(const string& str , const string& value , vector<string>& strtok_vec);
int main()
{

    string str = "  | 张三 | 3456123 , 湖南 | ";

    size_t pos = 0, posend = 0;

//    pos = str.find_first_not_of("| ," , 0);    // 搜索第一个，其实可以合并到下面 while里
//    posend = str.find_first_of("| ," , 0);
//    cout << str.substr(pos, posend) << endl;

    while ((pos = str.find_first_not_of("| ," , posend)) != string::npos) {
        posend = str.find_first_of("| ," , pos);
        cout << str.substr(pos, posend - pos) << endl;   // substr 第二个参数不是pos，是个长度
    }

    string text = "张三|3456123, 湖南\n"
                  "李四,4564234| 湖北\n"
                  "王小二, 4433253|北京\n";

    vector<string> IDTable;
    stringtok(text , "| ,\n" , IDTable);
    return 0;
}


/*     find_first_of() 查找第一个与value中的某值相等的字符
 *     find_first_not_of() 查找第一个与value中的所有值都不相等的字符
 *     find_last_of() 查找最后一个与value中的某值相等的字符
 *     find_last_not_of() 查找最后一个与value中的所有值都不相等的字符
 */

/*     basic_string substr( size_type index, size_type num = npos );
 *     substr()返回本字符串的一个子串，从index开始，长num个字符。
 *     如果没有指定，将是默认值 string::npos。
 *     这样，substr()函数将简单的返回从index开始的剩余的字符串。
 */


vector<string>& stringtok(const string& str , const string& value , vector<string>& strtok_vec)
{
    size_t pos = 0, posend = 0;
    while ((pos = str.find_first_not_of("| ," , posend)) != string::npos) {
        posend = str.find_first_of("| ," , pos);
        cout << str.substr(pos, posend - pos) << endl;
        strtok_vec.push_back(str.substr(pos, posend - pos));
    }
    return strtok_vec;
}

