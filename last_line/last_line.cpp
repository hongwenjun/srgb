//    功能：输出一个文件的最后n行数据，如果文件总行数不足n行，则显示全部数据
//    基本思路：把文件读入缓存，然后查找倒数第10个换行符
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 反查找Buf缓冲区中字符ch, 次数num次, 如果找不到返回NULL，少于num，返回Buf
const char* rfindBuf_chrTimes(char* Buf, size_t BufSize , int ch, size_t num);

int main(int argc, char* argv[])
{
    const char* fileName = argv[1];
    FILE* pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        perror("打开文件时错误!");
        return -1;
    }
    // 获得文件大小
    fseek(pFile, 0, SEEK_END);
    long fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // 把文件读到缓冲区
    char* fileBuf = new char[fileSize + 1];
    memset(fileBuf, 0, fileSize + 1);
    fread(fileBuf, fileSize, 1, pFile);

    const char* ps = NULL;
    ps = rfindBuf_chrTimes(fileBuf, fileSize , '\n', 10);
    // 测试代码
    printf("%s:文件大小%ld\n", fileName, fileSize);
    printf("%s\n", ps);
    delete[] fileBuf;
    fclose(pFile);
    return 0;
}
// 反查找Buf缓冲区中字符ch, 次数num次, 如果找不到返回NULL，少于num，返回Buf
const char* rfindBuf_chrTimes(char* Buf, size_t BufSize , int ch, size_t num)
{
    const char* ret = Buf;
    const char* ps = Buf + BufSize;
    size_t cnt = 0;
    while (ps > Buf) {
        if (*ps == ch)
            cnt++;
        if (cnt == num)
            return (ret = ++ps);
        ps--;
    }
    if (cnt == 0)
        return NULL;

    return ret;
} // 有些编辑器，会在文件最后一行插入一个空行，可能输出看上去是9行
