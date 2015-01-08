#include <iostream>
#include <zlib.h>
#include <string.h>

using namespace std;

#define BUFSIZE 64*1024
char buf[BUFSIZE];
int main()
{

    char str[] = "我是中国人!";

    // 写gz文件
    gzFile gz = gzopen("test.gz", "rw");
    gzwrite(gz, str, strlen(str));
    gzprintf(gz, "\n%d  %x  %f\n", 100, 100, 100.0);
    gzclose(gz);

    // 获取gz文件的数据大小
    int data_size = 0;  int cnt = 0;
    gzFile gzf = gzopen("test.gz", "rb");
    while ((cnt = gzread(gzf , buf ,   BUFSIZE))  > 0)
        data_size += cnt;
    cout << "数据大小: " << data_size << endl;
    gzrewind(gzf);

    // 读取gz文件到内存
    char* filebuf = new char[data_size + 1];
    filebuf[data_size] = 0;
    if (gzread(gzf , filebuf ,   data_size)  > 0)
        cout << "数据内容: " << filebuf << endl;


    gzclose(gzf);
    delete[] filebuf;
    return 0;
}

// ZEXTERN gzFile ZEXPORT gzopen OF((const char *path, const char *mode));
// ZEXTERN int ZEXPORT gzwrite OF((gzFile file, voidpc buf, unsigned len));
// ZEXTERN int ZEXPORT    gzrewind OF((gzFile file));
// ZEXTERN int ZEXPORT gzread OF((gzFile file, voidp buf, unsigned len));
// ZEXTERN int ZEXPORTVA gzprintf Z_ARG((gzFile file, const char *format, ...));
