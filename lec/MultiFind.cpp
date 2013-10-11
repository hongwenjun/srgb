#include <windows.h>
#include <stdio.h>
#include <string.h>

#define LINE_SIZE 255

void help(); // 帮助
BOOL IsFileExist(LPCTSTR lpFileName);  // 检查一个文件是否存在

int main(int argc, char* argv[])
{
    if ((1 == argc) || (2 == argc)) {    //错误输入处理
        help();
        return -1;
    } else if (!(IsFileExist(argv[1]) && IsFileExist(argv[2]))) {
        printf("文件错误：检查文件是否存在!\n");
        return -1;
    }

    FILE* keyfile = fopen(argv[1], "r"); // 关键字输入文件
    if (ferror(keyfile)) {
        printf("文件错误：不能打开输入文件: %s \n", argv[2]);
        return -1;
    }

    char key_value[255];
    char cmd_line[255] = {0};

    while (!feof(keyfile)) {  // 读取每一行关键字
        fscanf(keyfile, "%s", key_value);
        printf("查找: %s", key_value);

        strcpy(cmd_line, "find /N  \"");   //改成 find /C 统计找到的次数
        strcat(cmd_line, key_value);
        strcat(cmd_line, "\"  ");
        strcat(cmd_line, argv[2]);

        // cmd_line调用命令 find /N  "key_value" log.txt
        int ret = system(cmd_line);

        if (ret != 0)
            printf("没有找到: %s\n", key_value);

        printf("\n");
    }

    return 0;
}


// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName)
{
    WIN32_FIND_DATA fd = {0};
    HANDLE hFind = FindFirstFile(lpFileName, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    }
    return ((hFind != INVALID_HANDLE_VALUE) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

void help()
{
    printf("本工具可以按关键字文件在数据文件中搜索数据行  BY Hong Wenjun\n\n");
    printf("示例:  D:\\>MultiFind.exe  keyfile.txt  原始数据.txt  \n");
}
