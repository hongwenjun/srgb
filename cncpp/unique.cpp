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
