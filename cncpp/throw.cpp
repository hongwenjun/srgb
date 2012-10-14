#include <iostream>
using namespace std;

void test(bool bl)
{
    if (bl)
        throw "你好世界!";
    else
        throw "Hello world!";
}

int main(int argc, char* argv[])
{
    try {
        test(false);
    } catch (const char* s) {
        cout << s << endl;
        return -88;
    }
    return 0;
}


/*     try
 *     {
 *         cout << "Before throwing exception" << endl;
 *         throw 42;
 *         cout << "Shouldn't ever see this" << endl;
 *     }
 * 
 *     catch (int error)
 *     {
 *         cout << "Error: caught exception " << error << endl;
 *     }
 */
