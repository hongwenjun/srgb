#include <stdio.h>
#include <string>
#include <string.h>
#include <windows.h>


// gbk unicode utf-8 转换
const size_t maxsize = 4 * 1024;
wchar_t* charToWCHAR(wchar_t* wch, const char* czs)
{
    MultiByteToWideChar(CP_ACP, 0, czs, -1, wch, maxsize); // czs 转换到宽字节wch
    return wch;
}
char* WCHARTochar(char* czs , const wchar_t* wch)
{
    WideCharToMultiByte(CP_ACP, 0, wch, -1, czs, maxsize , NULL, NULL);
    return czs;
}
char* WCHARToUTF8(char* utf8 , const wchar_t* wch)
{
    WideCharToMultiByte(CP_UTF8, 0, wch, -1, utf8, maxsize , NULL, NULL);
    return utf8;
}
char* gbk_to_utf8(char* cstr)
{
    wchar_t tmp_wch[maxsize];
    charToWCHAR(tmp_wch, cstr);
    WCHARToUTF8(cstr, tmp_wch);
    return cstr;
}


// utf 编码成hex
char* utf_encode_hex(char* cstr);

// 从一行获取 姓名和电话号码
bool get_name_number(char* name, char* mobile_number, char* home_number , char* line);

void help(); // 调用使用帮助

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


    char* vcard_head = "BEGIN:VCARD\nVERSION:2.1\n";
    char* vcard_end = "END:VCARD\n";

    char* name_title = "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;";
    char* end_name = ";;;";

    char* f_name_title = "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";

    char*  mobile_number_title = "TEL;CELL:";
    char*  home_number_title = "TEL;HOME:";


    char name[512] = "通讯录";
    char mobile_number[64] = "1839593888";
    char home_number[64] = "610616"; // 可以存移动短号


    char* vcard_text =  new char[maxsize];  // 一个vcard文本
    char* line =  new char[maxsize];

    while (fgets(line, maxsize , input)) {  // 读取每一行
        if (strlen(line) < 10)
            continue;

        if (!get_name_number(name,  mobile_number,  home_number , line))
            continue;

        /*   把name 转换成 utf-8 ，然后 使用=EE=FF=EE取16进制进行编码 */
        gbk_to_utf8(name);
        utf_encode_hex(name);


        // 组成 一个vcard文本
        sprintf(vcard_text, "%s%s%s%s\n%s%s\n%s%s\n%s%s\n%s" , vcard_head,
                name_title , name , end_name ,
                f_name_title , name,
                mobile_number_title , mobile_number ,
                home_number_title  , home_number,
                vcard_end);

        fprintf(outfile, "%s", vcard_text);  // 写文件

        if (help_flag)  // 帮助测试
            break;
    }


    delete[] line;
    delete[] vcard_text;
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

bool get_name_number(char* name, char* mobile_number, char* home_number , char* line)
{
    char flag[] = "#;/"  ;  // 注释
    if ((line[0] == flag[0]) || (line[0] == flag[1]) || (line[0] == flag[2]))
        return false;

    name[0] = '\0' ;
    mobile_number[0] = '\0' ;
    home_number[0] = '\0' ;

    char* pch;
    char*   delimiters = " ,\t\n;\"";
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

    return true;
}

// utf 编码成hex
char* utf_encode_hex(char* cstr)
{
    unsigned char* pch = (unsigned char*) cstr;
    char byte_str[4] = {"\0"};
    char tmp[maxsize] = {"\0"};

    for (size_t i = 0; i != (strlen(cstr)) ; i++) {
        sprintf(byte_str, "=%0.2X" , *pch++);
        strcat(tmp, byte_str);
    }

    strcpy(cstr, tmp);
    return cstr;
}
