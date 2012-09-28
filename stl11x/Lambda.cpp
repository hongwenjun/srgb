#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream> //  定义string流
#include <fstream>
#include <string.h>
using namespace std;

struct pinyinUnit {     // 拼音单元数据结构
    char word[8] ;
    char py[32] ;
    int Pert;  // 单词 拼音 百分比
};
typedef vector<pinyinUnit> StringList;

bool cmp(const pinyinUnit a,  const pinyinUnit b)   //  比较函数cmp
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
    [](const pinyinUnit a,  const pinyinUnit b) { return strcmp(a.py , b.py) < 0; });

    //使用C++11 Lambda 函数，遍历输出
    for_each(sList.begin(), sList.end(),
    [](const pinyinUnit i) { cout << i.word << i.py << i.Pert << endl ;});


    fstream pyDictBin("pybin.dat" , ios_base::out | ios_base::binary);

//    for (auto i = 0; i != sList.size(); ++i)                // 遍历写文件
//        pyDictBin.write((char*) &sList[i] , sizeof(pinyinUnit));
//
//    for (auto it = sList.begin(); it != sList.end(); ++it)  // 遍历写文件
//        pyDictBin.write((char*) & (*it) , sizeof(pinyinUnit));   // 迭代器要先解引用，然后再取地址

    auto it = sList.begin();
    pyDictBin.write((char*) & (*it) , sizeof(pinyinUnit) * sList.size());  // 一次性把容器写文件

    return 0;
}

// 加载拼音字典file 到容器resultList
void loadPYdict(fstream& file, StringList& resultList)
{
    char word[64] = {0} ;
    char py[64] = {0} ;
    int Pert;  // 单词 拼音 百分比
    string line;
    pinyinUnit tmp ;
    memset(&tmp, 0 , sizeof(tmp));
    stringstream oss;    // 定义string流
    oss << file.rdbuf();

    while (getline(oss , line)) {   // 获取一行
        if (line.empty() || (line[0] == '#'))   // 空行或者注解行抛弃
            continue;

        sscanf(line.c_str(), "%s %s %d" , word, py , &Pert);    // 格式化读取一行
        strcpy(tmp.word , word);
        strcpy(tmp.py , py);
        tmp.Pert = Pert;
        resultList.push_back(tmp);  // 插入容器
    }
}

/*拼音字典文件： py.dat
中文  Zhōngwén  99
中国   Zhōngguó  1
浙江  Zhèjiāng   5
北京 Běijīng  9
上海 Shànghǎi   8
印刷 Yinshua  8
虫虫  Chongchong 4
*/

