#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct data_type {
    int age;
    char name[20];
} data;

int main()
{
    char* pstr = (char*) malloc(255);
    char* ps = new char[255];

    strcpy(pstr, "malloc 的用法");
    strcpy(ps, "new 的用法");
    printf("%s\t%s\n", pstr, ps);

    data* bob , *boy ;
    bob = (data*) malloc(sizeof(data)); // 分配一个结构
    boy = new data; // new 一个结构内存

    if (bob != NULL) {
        bob->age = 22;
        strcpy(bob->name, "Robert");
        printf("%s is %d years old\n", bob->name, bob->age);

        boy->age = 12;
        strcpy(boy->name, "Tom");
        printf("%s is %d years old\n", boy->name, boy->age);
    }

    free(bob);  delete boy;

    bob = (data*) malloc(sizeof(data) * 20); // 分配一个结构数组
    boy = new data[20]; // new 一个结构数组

    for (size_t i = 0 ; i != 20; ++i) {
        sprintf(pstr + 6 , "->%d", i);
        sprintf(ps + 3 , "->%d", i);

        bob[i].age = i;
        strcpy(bob[i].name, pstr);
        printf("%s is %d\t\t", bob[i].name, bob[i].age);

        boy[i].age = i;
        strcpy(boy[i].name, ps);
        printf("%s is %d\n", boy[i].name, boy[i].age);
    }

    free(pstr); delete ps;
    free(bob);  delete[] boy;
}
