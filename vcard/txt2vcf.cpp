#include <stdio.h>
#include <string>
#include <string.h>
#include <windows.h>
#include "utf8.h"




const size_t maxsize = 4 * 1024;



// utf 编码成hex
char* utf_encode_hex(char* cstr);

// 从一行获取 姓名和电话号码
bool get_name_number(char* name, char* mobile_number, char* home_number, char* line);

// 反序字符串
char* cs_strrev(char* string);
// 删除前后空白
char* strtrim(char* cs);
// 调用使用帮助
void help();

int main(int argc, char* argv[])
{

    bool help_flag = false;
    if (1 == argc) {  //错误输入处理
        help();
        help_flag = true;
    }

    FILE* input  = stdin;
    FILE* outfile = stdout;

    if (1 < argc) {
        input = fopen(argv[1], "r"); // 数据输入文件
        if (ferror(input)) {
            printf("文件错误：不能打开输入文件: %s \n", argv[1]);
            return -1;
        }
    }

    if (2 < argc)  // 如果没有输出文件，输出到屏幕
        outfile = fopen(argv[2], "w"); // 输出结果文件


    const char* vcard_head = "BEGIN:VCARD\nVERSION:2.1\n";
    const char* vcard_end = "END:VCARD\n";

    const char* name_title = "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;";
    const char* end_name = ";;;";

    const char* f_name_title = "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";

    const char*  mobile_number_title = "TEL;CELL:";
    const char*  home_number_title = "TEL;HOME:";


    char name[512] = "通讯录";
    char mobile_number[64] = "1839593888";
    char home_number[64] = "610616"; // 可以存移动短号


    char* vcard_text =  new char[maxsize];  // 一个vcard文本
    char* line =  new char[maxsize];

    while (fgets(line, maxsize, input)) {   // 读取每一行
        if (strlen(line) < 10)
            continue;

        if (!get_name_number(name,  mobile_number,  home_number, line))
            continue;

        /*   把name 转换成 utf-8 ，然后 使用=EE=FF=EE取16进制进行编码 */
        gb_to_utf8(name, name, strlen(name) * 3 / 2 + 1);

        //  printf("%s\n", name);

        utf_encode_hex(name);


        // 组成 一个vcard文本
        sprintf(vcard_text, "%s%s%s%s\n%s%s\n%s%s\n%s%s\n%s", vcard_head,
                name_title, name, end_name,
                f_name_title, name,
                mobile_number_title, mobile_number,
                home_number_title, home_number,
                vcard_end);

        fprintf(outfile, "%s", vcard_text);  // 写文件

        if (help_flag)  // 帮助测试
            break;
    }


    delete[] line;
    delete[] vcard_text;
    fclose(input);
    fclose(outfile);
    return 0;
}



#if(0)
/*  电话号码一行数据

BEGIN:VCARD
VERSION:2.1
N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;=E9=80=9A=E8=AE=AF=E5=BD=95;;;
FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=E9=80=9A=E8=AE=AF=E5=BD=95
TEL;CELL:1839593888
TEL;HOME:610616
END:VCARD

*/
#endif


void help()
{
    printf("本工具把通讯录 从文本TEXT转换成VCARD格式导入手机使用  BY Hong Wenjun\n\n");
    printf("示例 1 ：D:\\>txt2vcf.exe  TelText.txt \n");
    printf("示例 2 ：D:\\>txt2vcf.exe  TelText.txt  TelVcard.vcf \n");
    printf("\n输出文件不填，结果显示在屏幕上\a\n");
    printf("\n测试输入(按回车转换记录,按Ctrl+Z关闭程序):\n\n姓名\t手机号码\t移动短号\n");

}

bool get_name_number(char* name, char* mobile_number, char* home_number, char* line)
{
    char flag[] = "#;/"  ;  // 注释
    if ((line[0] == flag[0]) || (line[0] == flag[1]) || (line[0] == flag[2]))
        return false;

    // 清零
    name[0] = '\0' ;
    mobile_number[0] = '\0' ;
    home_number[0] = '\0' ;

    char* pch;
    char*   delimiters = ",;\t\n\"\'";
    pch = strtok(line, delimiters);
    if (pch != NULL) {
        sprintf(name, "%s", pch);
        pch = strtok(NULL, delimiters);
        if (pch != NULL) {
            sprintf(mobile_number, "%s", pch);
            pch = strtok(NULL, delimiters);
            if (pch != NULL)
                sprintf(home_number, "%s", pch);
        }
    }

    // 删除前后空格
    strtrim(name);
    strtrim(mobile_number);
    strtrim(home_number);

    return true;
}

// utf 编码成hex
char* utf_encode_hex(char* cstr)
{
    unsigned char* pch = (unsigned char*) cstr;
    char byte_str[4] = {"\0"};
    char tmp[maxsize] = {"\0"};

    for (size_t i = 0; i != (strlen(cstr)) ; i++) {
        sprintf(byte_str, "=%0.2X", *pch++);
        strcat(tmp, byte_str);
    }

    strcpy(cstr, tmp);
    return cstr;
}



// VC6.0 的_strrev 的源码 _strrev 不是标准库，所以可以应急自己写一个
char* cs_strrev(char* string)
{
    char* start = string;
    char* left = string;
    char ch;

    while (*string++) // 找到string 末位的'\0'，
        ;             // 因为上面 string++，实际指针在 '\0'的后一个
    string -= 2;      // 所以退2格，回到字符串最后一个字母
    while (left < string) {
        ch = *left;    // 通过临时变量 ch 进行交换
        *left++ = *string;
        *string-- = ch;
    }
    return (start);
}

char* strtrim(char* cs)
{
    char* ret = cs;
    char* pc = new char[strlen(cs) + 1];

    int pos = strspn(cs, "\t \n");      // 查找非空白处pos
    strcpy(pc, cs + pos);
    cs_strrev(pc);                        // 反序字符串
    pos = strspn(pc, "\t \n");          // 原来后面的空白，就变成了前面的空白
    strcpy(cs, pc + pos);
    cs_strrev(cs);                        // 再反序，还原回来

    delete[] pc;
    return ret;
}
