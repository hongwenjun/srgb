#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
using namespace std;

typedef vector<int> INTVEC ;          // 定义 整数容器 INTVEC
void sort(INTVEC& vi);
void print_intvec(INTVEC& vi);

int main()
{
    INTVEC vi = {4 , 5 , 3 , 1 , 8};  // 初始化 vi
    vi.push_back(6);                  // 插入一个数字 6
    sort(vi);
    vi.insert(vi.begin() + 3 , 99);   // 迭代器偏移3之前 插入一个数字 99

    int sum = 0;
    for_each(vi.begin(), vi.end(), [&sum](int & x) {  // 使用匿名函数遍历输出
        cout << x << "\t";
        sum += x;                      // 顺便统计总和
    });

    cout << "sum: " << sum << endl;

    sort(vi.begin(), vi.end(), [](int a , int b) {
        return a > b;             // 使用Lambda匿名函数 反排序
    });

    print_intvec(vi);             // 输出 INTVEC 容器

    return 0;
}

void sort(INTVEC& vi)             // 包装排序算法
{
    sort(vi.begin(), vi.end());
}

// 输出 INTVEC 容器
void print_intvec(INTVEC& vi)
{
    ostream_iterator<int> out_it(cout , " \t");
    copy(vi.begin(), vi.end(), out_it);
    cout << endl;
}

#if 0  // 输出结果
1       3       4       99      5       6       8       sum: 126
99      8       6       5       4       3       1
#endif

