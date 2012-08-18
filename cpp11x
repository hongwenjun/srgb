#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

int main()
{
    using namespace std;
    unordered_map<string, string> ss = { {"1->" , "ABC"} ,  {"2->" , "XYZ"} };
    ss.insert(pair<string, string>("3->" , "一二三"));
    ss.insert(make_pair("4->" , "一二三"));
    ss["5->"] = "Access element";

    for (auto it : ss) { // it 其实不是迭代器了，是容器物件的复制或者引用
        it.second = "修改" ;  // 没有修改
        cout << &it << "\t";
        cout << it.first << it.second << endl;
    }
    cout << endl;
    for (const auto  & it : ss) {
        cout << &it << "\t";
        cout << it.first << it.second << endl;
    }
    return 0;
}

#if(0)
for (auto it = ss.begin() ; it != ss.end() ; ++it)
    cout << it->first << it->second << endl;
#endif
