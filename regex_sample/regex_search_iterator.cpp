// regex_search.cpp
#include <iostream>
#include <string>
#include <regex>

int main()
{
    std::string s("this subject has a submarine as a subsequence");
    std::smatch m;
    std::regex e("\\b(sub)([^ ]*)");    // matches words beginning by "sub"

    // 方法 一
    std::regex_iterator<std::string::iterator> rit(s.begin(), s.end(), e);
    std::regex_iterator<std::string::iterator> rend;

    while (rit != rend) {
        std::cout << rit->str() << std::endl;
        ++rit;
    }

    // 方法 二
    while (std::regex_search(s, m, e)) {
        for (auto x : m)
            std::cout << x << " ";
        std::cout << std::endl;
        s = m.suffix().str();
    }

    return 0;
}
