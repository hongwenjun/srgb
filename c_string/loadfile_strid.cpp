#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

void loadfile_strid(FILE* in , char* ps_strid[7]);

int main()
{
    // char* ps_strid[7] ，指向常量区7个常量字串，不能修改指针指向的常量字符串
    char* ps_strid[] = {"12345",  "67890", "abcd", "xyz", "test", "vip",  "admin"  };

    char strid[7][32] ;     // 分配 7个空间用来存放字符串，窜长度限定在32之内
    // 严格地说，C++中没有多维数组，通常所指的二维数字组其实就是数组的数组
    // 第一维通常称为行，strid[7]，就是有7行，就是有7个空间单元

    for (int i = 0 ; i != 7; i++) {
        strcpy(strid[i] , ps_strid[i]);  // 把常量字串复制到分配的空间去
        ps_strid[i] = strid[i];          // 原来的指针指向分配的空间的地址
        cout << ps_strid[i] << endl;
    }

    // 下面来实现从文件中读取配置
    FILE* in = fopen("strid.txt" , "r");
    loadfile_strid(in , ps_strid);
    fclose(in);

    for (int i = 0 ; i != 7; i++)
        cout << ps_strid[i] << endl;
    return 0;
}


void loadfile_strid(FILE* in , char* ps_strid[7])
{
    char strid[7][32] = { {0}, {0}, {0}, {0}, {0}, {0}, {0}};
    fscanf(in , "%s %s %s %s %s %s %s" , strid[0], strid[1],
           strid[2], strid[3], strid[4], strid[5], strid[6]);

    for (int i = 0 ; i != 7; i++) {
        //   ps_strid[i] = strid[i];  这样是错误的，因为strid，是函数临时分配，函数结束就没有了
        strcpy(ps_strid[i] , strid[i]);  // ps_strid[i] 指针一定要有指向分配的空间的地址
    }
}


/*   filename:  strid.txt

Panzer   蘭公子二世   秦.狮王   似水無痕   kerou   H0rol4   GM

*/
