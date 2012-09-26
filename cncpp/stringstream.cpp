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
    fstream pyDict("py.dat" , ios_base::in);
    loadPYdict(pyDict, sList);

    for (auto it = sList.begin(); it != sList.end(); ++it)
        cout << *it << endl;

    return 0;
}


void loadPYdict(fstream& file, StringList& resultList)
{
    string word , py , Pert;  // 单词 拼音 百分比
    stringstream oss;    // 定义string流
    oss << file.rdbuf();
    while (oss) {
        oss >> word >> py >> Pert;
        resultList.push_back(word + py + Pert);
    }
    cout << "测试函数:" << word << py << Pert << endl; // 测试
}
