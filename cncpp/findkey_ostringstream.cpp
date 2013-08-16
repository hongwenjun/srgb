// c++文件流转字符串操作，查找要找的关键字
#include <iostream>
#include <fstream>
#include <sstream>

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

        // 查找关键字,第一次出现的位置
        if (string::npos != content.find(keyword)) {
            cout << "Find \"" << keyword << "\"!" << endl;
        } else {
            cout << "\"" << keyword << "\" Not found!" << endl;
        }
    } else {
        cerr << "Open file error!" << endl;
        return -1;
    }
    return 0;
}
