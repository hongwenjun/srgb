// c++文件流转字符串操作，查找关键字, 第一次出现的位置, 在文件的第 X 行
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>>

using namespace std;

int main(int argc, char* argv[])
{
    if (3 != argc) {
        cerr << "Usage : " << argv[0] << " filename keyword" << endl;
        return -1;    // 使用参数不对，退出
    }

    string filename(argv[1]);  // 输入文件
    string keyword(argv[2]);   // 搜索 关键字

    ifstream in(filename.c_str());
    if (in) {
        ostringstream os;   // 加载文件到sstream流
        os << in.rdbuf();
        string content(os.str()); // sstream流 转换成 字符串类

        // 查找关键字,第一次出现的位置, 在文件的第 X 行
        string::size_type pos = content.find(keyword);
        if (string::npos != pos) {
            // 在文件的第 X 行,找到关键字
            std::string::iterator it = content.begin();  // 指向字符串开始的迭代器
            int lines_count = count(it, it + pos, '\n') + 1;  // 使用迭代器
            cout << "Find \"" << keyword << "\" on Lines: " << lines_count << endl;

        } else {
            cout << "\"" << keyword << "\" Not found!" << endl;
        }
    } else {
        cerr << "Open file error!" << endl;
        return -1;
    }
    return 0;
}
