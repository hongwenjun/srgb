#include <stdio.h>
#include <string.h>
#include <ctype.h>
void printHexString(const void* buf , size_t size)
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
            sprintf(line + (11 + lineLength * 3), "; %s\n", text);
        }

        printf("%s", line);
    }
}

int main(int argc, char* argv[])
{
    const char* str = "printHexString函数打印字符串成16进制显示函数还没完善";
    printHexString(str , strlen(str));

    int a = 100;
    printHexString(&a , sizeof(a));

    double d = 100;
    printHexString(&d , sizeof(d));

    short arr[100];
    for (int i = 0; i != 100  ; ++i) {
        arr[i] = i + 100;
    }
    printHexString(arr , sizeof(arr));
}
