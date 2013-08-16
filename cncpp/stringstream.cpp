#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream> //  定义string流
#include <fstream>

using namespace std;

typedef vector<string> StringList;
void loadPYdict(fstream& file, StringList& resultList); // 读取文件到 容器里里

int main()
{
    StringList sList;
    fstream pyDict("py.dat" , ios_base::in);  // 输入数据文件 py.dat
    loadPYdict(pyDict, sList);

    for (auto it = sList.begin(); it != sList.end(); ++it) // C++ 11 的auto关键字，方便遍历容器
        cout << *it << endl;

    return 0;
}

// 读取文件到 容器里里
void loadPYdict(fstream& file, StringList& resultList)
{
    string word , py , Pert;  // 单词 拼音 百分比
    stringstream oss;    // 加载文件到sstream流
    oss << file.rdbuf(); // 文件缓冲区读到 sstream流
    while (oss) {
        oss >> word >> py >> Pert;
        resultList.push_back(word + py + Pert);
    }
    cout << "测试函数:" << word << py << Pert << endl; // 测试
}
