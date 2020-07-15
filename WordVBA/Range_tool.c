#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    const char * filename = argv[1];
    if (argc == 1)
        printf("\nUsage: %s  filename.txt \n", argv[0] );

    FILE* file = fopen(filename, "r");

    int cnt = 1;
    char key[64] = "NULL";
    char line[1024];

    while (fgets(line, 1024, file) != NULL) {

        if (strcmp(line, key) == 0) {
            //  printf("%d\t%s", cnt, line);
        } else {

            strcpy(key, line);
            printf("E%d  %sL%d:", cnt - 1,   key, cnt);
            // L开始单元格 E 结束
        }

        cnt++;
    }

    fclose(file);
    return 0;
}
