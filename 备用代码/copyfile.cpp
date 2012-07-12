#include <stdio.h>
//#include <iostream>

using namespace std;

int main()
{
    const char* filename = "main.cpp";
    FILE* f = fopen(filename, "rb");

// Get file size:
    fseek(f, 0, SEEK_END);
    const long N = ftell(f);
    fclose(f);

// 读取文件全部内容
    char* fil_buf = new char[N + 1];
    f = fopen(filename, "rb");
    fread(fil_buf, 1, N, f);
    fclose(f);
    fil_buf[N] = '\0';

// Write new file:
    f = fopen("newfile.txt", "wb");
    fwrite(fil_buf, 1, N , f);
    fclose(f);

    fprintf(stdout, "%s" , fil_buf);

    return 0;
}
