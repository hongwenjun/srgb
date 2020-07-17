#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    const char* filename = argv[1];
    if (argc == 1) {
        printf("\nUsage: %s  filename.txt \n\n  输出信息 >> L1:E8  L开始行  E结束行\n", argv[0]);
        return -1;
    }

    FILE* file = fopen(filename, "r");

    int cnt = 1;
    char key[64] = "NULL";
    char line[1024];
    char* enter;

    while (fgets(line, 1024, file) != NULL) {
        enter = strchr(line, '\n');
        if (enter)
            *enter = '\0';

        if (strcmp(line, key) == 0) {
            //  printf("%d\t%s", cnt, line);
        } else {

            strcpy(key, line);
            printf("E%d\n%s    L%d:", cnt - 1,   key, cnt);
            // L开始单元格 E 结束
        }

        cnt++;
    }
    printf("E%d\n", cnt - 1);
    fclose(file);
    return 0;
}
