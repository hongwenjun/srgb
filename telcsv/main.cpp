#include <stdio.h>
#include <string.h>

int main()
{


    FILE* pFile;
    char* pch;
    char str [255];

    pFile = fopen("tel.csv" , "r");
    if (pFile == NULL) {
        perror("Error opening file");
        return -1;
    }

    while (! feof(pFile)) {
        fgets(str , 255 , pFile);
        pch = strtok(str, ",\n");
        while (pch != NULL) {
            printf("%s\t", pch);
            pch = strtok(NULL, ",\n");
        }
        printf("\n");
    }
    fclose(pFile);
    return 0;
}
/* 输出结果
姓名    办电    宅电    手机    虚拟网
卢小珍  34521111        88903453        13537902123     332123
童云龙  34521113        88927528        13503793985     373985
童万贞  34521112        34523377        13803778530     338530
童万贞 // 程序有BUG 最后一行会再输出
*/

