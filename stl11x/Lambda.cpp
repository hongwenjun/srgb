#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream> //  定义string流
#include <fstream>
#include <string.h>
using namespace std;

struct pyUnit {     // 拼音单元数据结构
    string word ;
    string py ;
    int Pert;  // 单词 拼音 百分比
};
typedef vector<pyUnit> StringList;

bool cmp(const pyUnit a,  const pyUnit b)   //  比较函数cmp
{   return a.Pert < b.Pert;   }

void loadPYdict(fstream& file, StringList& resultList);  // 加载拼音字典file 到容器resultList

int main()
{
    StringList sList;       //  定义装字典的容器
    fstream pyDict("py.dat" , ios_base::in);
    loadPYdict(pyDict, sList);  // 加载字典到容器

    sort(sList.begin(), sList.end() , cmp);     // 按Pert排序

    for (auto it = sList.begin(); it != sList.end(); ++it)  // 遍历输出
        cout << it->word << it->py << it->Pert << endl;

    cout << "=========================" << endl;

    //使用C++11 Lambda 函数，按拼音排序
    sort(sList.begin(), sList.end() ,
    [](const pyUnit a,  const pyUnit b) { return a.py < b.py; });

    //使用C++11 Lambda 函数，遍历输出
    for_each(sList.begin(), sList.end(),
    [](const pyUnit i) { cout << i.word << i.py << i.Pert << endl ;});

    return 0;
}

// 加载拼音字典file 到容器resultList
void loadPYdict(fstream& file, StringList& resultList)
{
    char word[64] ;
    char py[64] ;
    int Pert;  // 单词 拼音 百分比
    string line;
    pyUnit tmp;
    stringstream oss;    // 定义string流
    oss << file.rdbuf();

    while (getline(oss , line)) {   // 获取一行
        if (line.empty() || (line[0] == '#'))   // 空行或者注解行抛弃
            continue;

        sscanf(line.c_str(), "%s %s %d" , word, py , &Pert);    // 格式化读取一行
        tmp.word = word;
        tmp.py = py;
        tmp.Pert = Pert;
        resultList.push_back(tmp);  // 插入容器
    }
}

/*拼音字典文件： py.dat
中文  Zhōngwén  99
中国  Zhōngguó  1
浙江  Zhèjiāng   5
北京  Běijīng  9
上海  Shànghǎi   8
*/

