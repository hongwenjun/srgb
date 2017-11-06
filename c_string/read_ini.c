#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 读取配置文件中的值
char* read_ini(char* filename, char* key, char* value)
{
    char* line = (char*) malloc(1024);
    char* pch = NULL;
    FILE* file = fopen(filename, "r");
    while (fgets(line, 1024, file) != NULL) {

        // 跳过没有'='号 和 有';' 的注解行
        if (strchr(line, '=') == NULL)
            continue ;

        pch = strtok(line, "= \t");
        if (pch != NULL) {
            if (strncmp(key, pch, strlen(key)) == 0) {
                pch = strtok(NULL, ";'/");
                strcpy(value, pch);
                //     printf("%s\n", pch);
            }
        }
    }

    free(line);
    fclose(file);
    return value;
}


int main()
{
    char filename[] = "txt.txt";
    char key[512] = "File";
    char value[512]="预设值";

    printf("%s ----> %s\n", key, read_ini(filename, key, value));

    return 0;
}

/*******  txt.txt *******
[Project]
RunMode=0
InitMode=0
RunAddr=0xa0000200
FileNum=2
[File0]
File=all.bin
Type=1
;Addr=0xa0010000
Addr=0xa0220000
[File1]
File=  eromclient.abs
Type=1
Addr=0xa0000200
[AutoRun]
AutoRun0=wm 0xa0000100 0
AutoRun1=wm 0xa0000104 0x200000
AutoRUn2=wm 0xa0000108 0xa0220000
AutoRun3=wm 0xa000010c 0xa03f0000
************************/
