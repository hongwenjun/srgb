#include<stdio.h>
#include<string.h>
void help(); // 调用使用帮助

#define LINE_SIZE 255

int main(int argc, char* argv[])
{
    if ((1 == argc) || (2 == argc)) {    //错误输入处理
        help();
        return -1;
    }
    FILE* input = fopen(argv[2], "r"); // 数据输入文件
    if (ferror(input)) {
        printf("文件错误：不能打开输入文件: %s \n", argv[2]);
        help();
        return -1;
    }
    FILE* outfile;
    if (3 < argc)  // 如果没有输出文件，输出到屏幕
        outfile = fopen(argv[3], "w"); // 输出结果文件
    else
        outfile = stdout;

    char line[LINE_SIZE]; // 读取一行 字符串
    char* ps;
    char* Value = argv[1];

    printf("正在搜索文件: %s  ...........\a\n", argv[2]);
    while (fgets(line, LINE_SIZE , input)) {  // 读取每一行
        if (ps = strstr(line, Value)) {  // strstr 功能：返回一个指针，它指向字符串中首次出现于Value中的位置，如果没有找到，返回NULL
            ps = line; // ps   恢复到行首
            fputs(ps , outfile);  // 输出结果
        }

    }
    return 0;
}

void help()
{
    printf("本工具可以从文件中按关键字搜索数据行 [C语言版本] BY Hong Wenjun\n\n");
    printf("示例 1 ：D:\\>LineExtraction.exe  \"关键字\"  D:\\原始数据.txt  \n");
    printf("示例 2 ：D:\\>LineExtraction.exe  \"关键字\"  D:\\原始数据.txt  D:\\数据结果.txt\n");
    printf("\n输出文件不填，结果显示在屏幕上\a\n");
}
