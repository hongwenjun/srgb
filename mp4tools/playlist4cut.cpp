#include <cstring>
#include <cstdio>
#include <cctype>
#include <vector>
#include <algorithm>

#define LINE_SIZE 1024
struct Timestamp {
    char start[64];
    char end[64];
    char name[512];
};

char * strtrim(char *s);

int main(int argc, char* argv[])
{
    if (2 > argc) {
        puts("Usage: playlist4cut.exe  playlist.txt  [timelist.txt] ");
        return -1;
    }

    // 时间戳数据
    Timestamp one = { "00:08", "01:18", "Audiomachine - Age of Dragons" };

    std::vector<Timestamp>  vec_one;
    char line[LINE_SIZE]; // 读取一行 字符串
    char* pch;

    FILE* input = fopen(argv[1], "r");
    if (input == NULL)
        return -1;

    while (fgets(line, LINE_SIZE, input)) {
        if (pch = strtok(line, " \t\n")) {
            strcpy(one.start, pch);
            pch = strtok(NULL, "\n\r");
            pch = strtrim(pch);
            strcpy(one.name, pch);

            vec_one.push_back(one);   // 把读取的时间戳装载到容器
        }
    }

    // 副本偏移，用来修改 Timestamp.end
    std::vector<Timestamp>  vec_copy = vec_one;
    vec_copy.push_back(one);
    vec_copy.erase(vec_copy.begin());


    FILE* output;
    if (3 == argc)  // 如果没有输出文件，输出到屏幕
        output = fopen(argv[2], "w"); // 输出结果文件
    else
        output = stdout;

    for (auto it = vec_one.begin(), itcp = vec_copy.begin(); it != vec_one.end(); ++it, ++itcp) {
        strcpy(it->end, itcp->start);
        fprintf(output, "::M4ACUT::  %s  %s  \"%s.m4a\"\n", it->start, it->end, it->name);
    }

    fprintf(output, "\n## 注意最后一行时间结束时间要手工修改  ##");

    return 0;
}

// strtrim 去掉字符串前后的空格和制表符
char* strtrim(char* s)
{
    char* p = s;
    while (isspace(*p))
        ++p;

    char* end = s + strlen(s) - 1;
    while (isspace(*end))
        --end;
    *(end + 1) = '\0';

    strcpy(s, p);
    return s;
}
