#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

typedef  char*  c_string ;
int loadfile_strid(FILE* in , c_string* ps_strid , int max);

int main()
{
    char InstrumentID[7][32];
    c_string ppInstrumentID[7];
    for (int i = 0; i != 7 ; ++i) {
        ppInstrumentID[i] = InstrumentID[i];
    }

    // 下面来实现从文件中读取配置
    FILE* in = fopen("strid.txt" , "r");

    int readflag = loadfile_strid(in , ppInstrumentID , 7);
    if (readflag == 7) {
        cout << "这行有数据" << endl;
    }
    readflag = loadfile_strid(in , ppInstrumentID , 7);
    if (readflag == -1)
        cout << "这行注解" << endl;

    readflag = loadfile_strid(in , ppInstrumentID , 5);
    if (readflag > 0) {
        c_string* it = ppInstrumentID;     // auto 自动判别类型 char**
        c_string* end = ppInstrumentID + 5;
        while (it != end)    // 迭代器遍历输出
            cout << *it++ << endl;
    }

    fclose(in);
    return 0;
}

// 从配置文件 in 里读取数据，存放到c_string* ps_strid 指向空间，max是一行中数据数量
int loadfile_strid(FILE* in , c_string* ps_strid , int max)
{
    // 读取一行
    char line[512] = {0};
    fgets(line, 512, in);
    if (strstr(line, "#"))
        return -1; // 注解行 返回 -1
    int index = 0;
    char* pch;

    // 切割后，然后复制到指针指向分配的空间的地址
    pch = strtok(line, " ,:;-");
    while (pch != NULL && max--) {
        strcpy(ps_strid[index] , pch);
        pch = strtok(NULL, " ,:;-");
        index++;
    }

    return index;  // 没有切割返回0  , 可以判断切换后数量是否正确
}


/*   filename:  strid.txt     分割符号 " ,:;-"

   Panzer ,   蘭公子二世  : 秦.狮王  ;  似水無痕 -  kerou   H0rol4   GM
#  注解行  ;  似水無痕 -  kerou   H0rol4   GM  Panzer ,   蘭公子二世  :
  秦.狮王  ;  似水無痕 -  kerou   H0rol4   GM  Panzer ,   蘭公子二世  :

*/
