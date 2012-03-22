#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include <string.h>

using namespace std;

int main()
{
    ifstream infile("NAMESPACE_STD.CPP");
    set<string> keyword_set;
    string keyword;
    // 装载关键字到set容器
    while (infile >> keyword)
        keyword_set.insert(keyword);

    infile.close();
    infile.clear();
    infile.open("src.cpp");
    stringstream oss;
    oss << infile.rdbuf();
    string text = oss.str();   // 修改输入文本 text 中非字母为空格
    for (size_t i = 0 ; i != text.size() ; i++) {
        if (!isalpha(text[i]))
            text[i] = ' ';
    }
    oss.clear();
    oss.str(text);

    // 输出结果到文件
    ofstream outfile("keyword.txt");
    int cnt = 0;
    while (oss >> keyword) {
        if (keyword_set.count(keyword) > 0) {
            outfile << keyword << " " ;
            if ((++cnt % 20) == 0)
                outfile << endl ;
        }
    }
    cout << "\n找到单词总次数:" << cnt << "次\n";
    return 0;
}
