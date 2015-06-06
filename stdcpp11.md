使用C++11 添加参数 -USTRICT\_ANSI
就可以使用 非标准函数了
-std=c++11 -USTRICT\_ANSI

```
g++.exe -std=c++11 -U__STRICT_ANSI__  -Wall -fexceptions  -g -fexec-charset=gbk -finput-charset=UTF-8   -c main.cpp -o main.o
```

或者把 #undef  STRICT\_ANSI
写到头文件前面 #include 

&lt;cstring&gt;



```
#include <iostream>
#undef  __STRICT_ANSI__
#include <cstring>

using namespace std;

int main()
{

    char str[] = "123";
    cout << strrev(str) << endl;
    return 0;
}

```