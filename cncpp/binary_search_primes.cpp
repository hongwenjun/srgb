#include <stdio.h>
#include <stdlib.h>
int* pt = NULL;  // primes_table
int pt_size = 0; // primes_table  数量大小

int init_primes_table(void)
{

    FILE* pFile;

    pFile = fopen("primes_table.bin", "rb");

    if (pFile == NULL) {
        fputs("primes_table.bin error", stderr);    // 如果文件错误，退出1
        exit(1);
    }

// obtain file size:  获得文件大小
    fseek(pFile, 0, SEEK_END);   // 指针移到文件末位
    int file_size = ftell(pFile);  // 获得文件长度
    rewind(pFile);  // 函数rewind()把文件指针移到由stream(流)指定的开始处, 同时清除和流相关的错误和EOF标记

    int pt_size =  file_size / sizeof(int); // 质数个数

    pt = new int[pt_size]; // 分配 质数表内存

// copy the file into the buffer:  该文件复制到缓冲区
    int  result = fread(pt, sizeof(int), pt_size, pFile); // 返回值是读取的内容数量
    if (result != pt_size) {
        fputs("Reading error", stderr);    // 返回值如果不和文件大小，读错误
        exit(3);
    }
    fclose(pFile);

    return pt_size;
}

/* 非递归算法   二分法查找
 * 算法：当数据量很大适宜采用该方法。
 * 采用二分法查找时，数据需是排好序的。
 * 主要思想是：（设查找的数组区间为array[low, high]）
 */
int binary(int* a, int key, int n)
{   int left = 0, right = n - 1, mid = 0;
    mid = (left + right) / 2;
    while (left < right && a[mid] != key) {
        if (a[mid] < key) {
            left = mid + 1;
        } else if (a[mid] > key) {
            right = mid;
        }
        mid = (left + right) / 2;
    }
    if (a[mid] == key)
        return mid;
    return -1;}

// 判断是否质数  9999991 以内
bool isprimes(int x)
{   int flag = binary(pt, x, pt_size);
    if (flag == -1)
        return false;
    return true;}

int main(int argc, char* argv[])
{   pt_size = init_primes_table();
    int primes_max = pt[pt_size - 1];
    printf("1000万以内质数判断，最大数 %d\n", primes_max);
    printf("%d is %d\n",  9710521, isprimes(9710521));
    printf("%d is %d\n",  2542733, binary(pt, 2542733, pt_size));
    return 0;}
