 // regex_search example
#include <iostream>
#include <string>
#include <regex>

int main()
{
    std::string s("this subject has a submarine as a subsequence");
    std::smatch m;
    std::regex e("\\b(sub)([^ ]*)");    // matches words beginning by "sub"  //GCC未支持


    std::cout << "目标序列: " << s << std::endl;
    std::cout << "正则公式: /\\b(sub)([^ ]*)/" << std::endl;
    std::cout << "The following matches and submatches were found:" << std::endl;

    while (std::regex_search(s, m, e)) {
        // 输出方式1
        for (auto i = 0 ; i != m.size() ; ++i)
            std::cout << m[i] << " ";
        std::cout << std::endl;
        // 输出方式2
        for (auto it = m.begin() ; it != m.end() ; ++it)
            std::cout << *it << " ";
        std::cout << std::endl;

//      for (auto x:m) std::cout << x << " ";   // VC2010 不支持

        // 格式化输出
        std::cout << m.format("公式匹配: [$0].\n");
        std::cout << m.format("子公司S1和S2匹配:  [$1] and [$2].\n\n");

        s = m.suffix().str();  // 返回末端，作为新的搜索的开始
    }

    return 0;
}
