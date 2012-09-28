#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

using namespace std;

class timer    // 计时类
{
public:
    timer() {_start_time = clock();}         // 时间起点
    void restart() {_start_time = clock();}  // 重置时间起点
    double elapsed() const {return double(clock() - _start_time) / CLOCKS_PER_SEC;}  // 时差
private:
    clock_t _start_time;
};

int main()
{
    ofstream fcout("a.txt");
     timer t;                     // 计时类
    stringstream oss;
    for (int i = 0; i != 1000000; ++i) {
        oss <<  i << endl;
    }
    fcout << oss.str();

    cout << "Time: " << t.elapsed() << endl;

    t.restart();
    for (int i = 0; i != 1000000; ++i) {
        fcout <<  i << endl;
    }
    cout << "Time: " << t.elapsed() << endl;

    return (0);
}
