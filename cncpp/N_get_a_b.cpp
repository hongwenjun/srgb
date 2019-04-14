#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    int a = 1 ;
    int b = 1;
    int x, m;
    int N =99;

    cout << "计算  1/N = 1/a  + 1/b  ; a和b是自然数不能相等;"
         << endl  << "N 等于 " << N << endl;

    for (int i = 0; i != 100000 ; i++) {
        for (int j = 0; j != 1000 ; j++) {

            x = a * b / (a + b);   // N = a * b / (a + b);
            m = a * b % (a + b);   // 整除

            if ((x == N) && (m == 0) && (a > b))
                cout << a <<  "\t" << b << endl;
            b++;
        }

        a++;
        b = 1;
    }

    return 0;
}
