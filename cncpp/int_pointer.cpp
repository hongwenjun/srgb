#include <iostream>  // C++使用标准的文件头 iostream.h 已经不用了
using namespace std; // 所以用使用 std 标准命名空间

int main()
{
    int a[5] = {1, 2, 3, 4, 5};
    int* pa, *pb;
    pa = a;  // a[0] 的地址
    pb = a + 4;  // a[4] 的地址
    cout << "pa->a[0] = " << *pa << endl;   // * 解引用
    cout << "pb->a[4] = " << *pb << endl;

    pa += 2;  // 指针偏移2
    cout << "pa->a[2] = " << *pa << endl;

    cout << "pa++ ->a[3] = " << *(++pa) << endl;   // 先++ 然后 *解引用
    int c = pb - pa; // pb->a[4] 和 pa->a[3] 两个int指针地址相差1 ，而不是( 5-4=1 ) ,开始还一下没看懂
    cout << "pb-pa = " << c << endl;
}
/* 输出
pa->a[0] = 1
pb->a[4] = 5
pa->a[2] = 3
pa++ ->a[3] = 4
pb-pa = 1

*/
