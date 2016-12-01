#include<stdio.h>
#include<string.h>
void help(); // 调用使用帮助

#define LINE_SIZE 1024

int main(int argc, char* argv[])
{
    if (1 == argc) {    //错误输入处理
        help();
        return -1;
    }
    FILE* input = fopen(argv[1], "r"); // 数据输入文件
    if (ferror(input)) {
        printf("文件错误：不能打开输入文件: %s \n", argv[1]);
        help();
        return -1;
    }
    FILE* outfile;
    if (2 < argc)  // 如果没有输出文件，输出到屏幕
        outfile = fopen(argv[2], "w"); // 输出结果文件
    else
        outfile = stdout;

    char line[LINE_SIZE]; // 读取一行 字符串
    char* ps;
    char* Value = argv[1];

    int cnt_line = 0;
    bool odd_number = true;
    int fraction = 2;   // 默认输出奇数行
    if (3 < argc) {     // 设置分数
        int n = 2;  sscanf(argv[3], "%d", &n);
        if (n > 2)
            fraction = n;
    }

    while (fgets(line, LINE_SIZE, input)) {   // 读取每一行

        if (odd_number =  !(cnt_line % fraction))
            fputs(line, outfile);   // 输出结果

        cnt_line++;
    }

    return 0;
}

void help()
{
    printf("本工具获取文件(1/N)行内容\n");
    printf("Usage: fraction_line.exe  input.txt  new.txt  10\n");

    printf("\n输出文件不填，结果显示在屏幕上\a\n");
}
