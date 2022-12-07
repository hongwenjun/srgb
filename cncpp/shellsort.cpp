#include <stdio.h>
void shellsort(int v[], int n)
{
    int gap, i, j, temp;
    for (gap = n / 2; gap > 0 ; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i - gap; j >= 0 && v[j] > v[j + gap]; j -= gap) {
                temp = v[j];
                v[j] = v[j + gap];
                v[j + gap] = temp;
            }
}

int main()
{
    int v[] = {5, 7, 1, 66, 33, 22, 11, 30};

    shellsort(v, 8);
    for (int i = 0; i != 8 ; i++)
        printf("%d ", v[i]);

}

// 1 5 7 11 22 30 33 66
