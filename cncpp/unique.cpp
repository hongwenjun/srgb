#include <cstdio>
#include <algorithm>

int main(int argc, char** argv)
{
    int a[] = {1, 1, 1, 2, 2, 3, 3, 4, 4, 5};
    int size = sizeof a / sizeof * a;
    auto ps = std::unique(a, a + size);

    for (int i = 0 ;  i != (ps - a); ++i)
        printf("%d", a[i]);

    return 0;
}


#include<stdio.h>  
int main()
{
    int i;
    char a[100];
    gets(a);
    for (i = 0; a[i] != '\0';){
        if (a[i] == ' '){//如果是空格输出一个空格，并向后遍历直到不是空格为止，中间不输出
            printf(" ");
            while (a[i] == ' ') i++;
        }
        else{//如果字符不是空格直接输出并向后遍历
            printf("%c", a[i]);
            i++;
        }
    }
    printf("\n");
    return 0;
}

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
 
int main() 
{
    // remove duplicate elements (normal use)
    std::vector<int> v{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v.begin(), v.end()); // 1 1 2 2 3 3 3 4 4 5 5 6 7 
    auto last = std::unique(v.begin(), v.end());
    // v now holds {1 2 3 4 5 6 7 x x x x x x}, where 'x' is indeterminate
    v.erase(last, v.end()); 
    for (int i : v)
      std::cout << i << " ";
    std::cout << "\n";
 
    // remove consecutive spaces
    std::string s = "wanna go    to      space?";
    auto end = std::unique(s.begin(), s.end(), [](char l, char r){
        return std::isspace(l) && std::isspace(r) && l == r;
    });
    // s now holds "wanna go to space?xxxxxxxx", where 'x' is indeterminate
    std::cout << std::string(s.begin(), end) << '\n';
}


#include <iostream>
#include <algorithm>
#include <string.h>
 
using namespace std;
 
int main()
{
    char buf[] = "   1    22   333444455555555555555555       678 9";
 
    // Result: " 1 2 345 678 9"
    auto ps = unique(buf, buf + strlen(buf));
    *ps = '\0';
 
    cout << buf << endl;
    return 0;
}
