#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

void sort(vector<int>& vi);

int main()
{
    vector<int> vi = {4 , 5 , 3 , 1 , 8};
    vi.push_back(6);
    sort(vi);
    vi.insert(vi.begin() + 3 , 99);

    int sum = 0;
    for_each(vi.begin(), vi.end(),
        [&sum](int & x) {
            cout << x << endl;
            sum += x;
        });
    
    cout << "sum: " << sum << endl;
    return 0;
}

void sort(vector<int>& vi)
{
    sort(vi.begin(), vi.end());
}

