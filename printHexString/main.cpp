#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void printBinStr(const void* buf , size_t size);
void printHexStr(const void* buf , size_t size);

int main(int argc, char* argv[])
{
    char arr[100];
    for (int i = 0; i != 100  ; ++i) {
        arr[i] = i+ '1';
    }
    printBinStr(arr , sizeof(arr));

    putchar('\n');
// 打印文件
    const char* filename =  "main.cpp";
    if (argc == 2)
        filename = argv[1];

    FILE *f = fopen(filename, "rb");
    if (!f) {
        std::cerr << "Error: Cannot open File "<< std::endl;
        return -1;
    }
        // Get file size:
    fseek(f, 0, SEEK_END);
    const long N = ftell(f);
    fclose(f);

    f = fopen(filename, "rb");
    char* buf = new char[N+1];
    fread(buf,1,N,f);
    buf[N]= '\0';

    printHexStr(buf , N);



    return 0;
}

void printBinStr(const void* buf , size_t size)
{
    unsigned char* str = (unsigned char*)buf;
    char line[512] = {0};
    const size_t lineLength = 6; // 每行6个字符
    char text[24] = {0};
    char sbin[10] = {0};
    char* pc;
    int textLength = lineLength;


    for (size_t ix = 0 ; ix < size ; ix += lineLength) {
        sprintf(line, "%.8xh: ", ix);
// 打印二进制
        for (size_t jx = 0 ; jx != lineLength ; jx++) {

        itoa(* (str + ix + jx),   sbin,   2);       //转换成字符串，进制基数为2

            if (ix + jx >= size) {
                sprintf(line + (11 + jx * 9), "         "); // 处理最后一行空白
                if (ix + jx == size)
                    textLength = jx;  // 处理最后一行文本截断
            } else
                sprintf(line + (11 + jx * 9), "%08s ", sbin);
        }
// 打印字符串
        {
            memcpy(text, str + ix, lineLength);
            pc = text;
            while (pc != text + lineLength) {
                if ((unsigned char)*pc < 0x20) // 空格之前为控制码
                    *pc = '.';                 // 控制码转成'.'显示
                pc++;
            }
            text[textLength] = '\0';
            sprintf(line + (11 + lineLength * 9), "; %s", text);
        }

        printf("%s\n", line);
    }
}

void printHexStr(const void* buf , size_t size)
{
    unsigned char* str = (unsigned char*)buf;
    char line[512] = {0};
    const size_t lineLength = 16; // 8或者32
    char text[24] = {0};
    char* pc;
    int textLength = lineLength;

    for (size_t ix = 0 ; ix < size ; ix += lineLength) {
        sprintf(line, "%.8xh: ", ix);
// 打印16进制
        for (size_t jx = 0 ; jx != lineLength ; jx++) {
            if (ix + jx >= size) {
                sprintf(line + (11 + jx * 3), "   "); // 处理最后一行空白
                if (ix + jx == size)
                    textLength = jx;  // 处理最后一行文本截断
            } else
                sprintf(line + (11 + jx * 3), "%.2X ", * (str + ix + jx));
        }
// 打印字符串
        {
            memcpy(text, str + ix, lineLength);
            pc = text;
            while (pc != text + lineLength) {
                if ((unsigned char)*pc < 0x20) // 空格之前为控制码
                    *pc = '.';                 // 控制码转成'.'显示
                pc++;
            }
            text[textLength] = '\0';
            sprintf(line + (11 + lineLength * 3), "; %s", text);
        }

        printf("%s\n", line);
    }
}

