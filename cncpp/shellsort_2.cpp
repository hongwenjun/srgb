#include <stdio.h>
#include <algorithm>
void shellsort(int *v, int n)
{
    int gap, i, j;
    for (gap = n / 2; gap > 0 ; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i - gap; j >= 0 && v[j] > v[j + gap]; j -= gap) {
                std::swap(v[j], v[j + gap]);
            }
}

int main()
{
    int v[] = {24,51,63,49,79,13,95,28,38,83,70,58,33,30,97,59,85,84,93,77,80,41,65,
              3,100,7,43,18,94,88,61,26,82,47,62,2,73,32,15,72,96,78,67,90,71,48,68,
              27,4,40,36,16,37,44,9,52,86,39,1,53,20,6,21,8,99,34,12,57,19,60,69,35,
              64,42,10,14,81,98,91,17,50,75,23,29,45,76,92,5,31,22,87,25,55,11,66,46,
              54,56,89,74};

    int total = sizeof(v)/sizeof(*v);
    shellsort(v, total);

    for (int i = 0; i != total ; i++)
        printf("%d ", v[i]);

}

// 1 2 3 4 5 6 ...  98 99 100