#include <iostream>
using namespace std;

void test(bool bl)
{
    if (bl)
        throw "你好世界!";    
    else
        throw "Hello world!";  // 抛出异常信息，C风格字符串
}

int main(int argc, char* argv[])
{
    try {                   // 异常测试
        test(false);
    } catch (const char* s) {  // 只能只读使用异常信息
        cout << s << endl;
        return -88;     // 直接结束程序
    }
    return 0;
}


/*     try
 *     {
 *         cout << "Before throwing exception" << endl;
 *         throw 42;   // 抛出异常号:数字
 *         cout << "Shouldn't ever see this" << endl;
 *     }
 * 
 *     catch (int error)  // 获得异常号的副本，输出
 *     {
 *         cout << "Error: caught exception " << error << endl;
 *     }
 */
