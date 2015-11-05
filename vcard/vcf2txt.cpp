#include <stdio.h>
#include <string>
#include <string.h>
#include <windows.h>


#include "utf8.h"
#include "quotedprintable.h"

#include <iostream>
#include <string>
using namespace std;

void help(); // 调用使用帮助
long get_file_size(FILE* file); //获得文件大小

// 内存匹配函数memfind
char* memfind(const char* buf, const char* tofind, size_t len);

// 内存搜索VCARD 提取标记之间字符，装载到 vcard_text[]
bool menSearch_vcard(const char* vps, size_t count,
                     const char* flag_beg, const char* flag_end,
                     char* vcard_text, size_t* offset);


int main(int argc, char* argv[])
{

    if (1 == argc) {  //错误输入处理
        help();
        return -1;
    }

    FILE* outfile = stdout;

    FILE* input  = fopen(argv[1], "r"); // 数据输入文件
    if (ferror(input)) {
        printf("文件错误：不能打开输入文件: %s \n", argv[1]);
        return -1;
    }


    if (2 < argc)  // 如果没有输出文件，输出到屏幕
        outfile = fopen(argv[2], "w"); // 输出结果文件


    const char* vcard_head = "BEGIN:VCARD\n";
    const char* vcard_end = "END:VCARD\n";

    const char* f_name_title = "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
    const char*  mobile_number_title = "TEL;CELL:";
    const char*  home_number_title = "TEL;HOME:";
    char name[512] = "";
    char mobile_number[64] = "";
    char home_number[64] = ""; // 可以存移动短号

    // 读取文件到内存
    size_t file_size = get_file_size(input);
    char* inbuf = new char[file_size + 1];
    memset(inbuf, 0, file_size + 1);
    fread(inbuf, file_size, 1, input);



    char* vcard_text = new char[8 * 1024];
    memset(vcard_text, 0, 8 * 1024);
    size_t offset = 0;

    const char* vps = inbuf;
    size_t count = file_size;

    bool is_vcard = menSearch_vcard(vps, count, vcard_head, vcard_end,  vcard_text, &offset);


    if (is_vcard) {
        fprintf(outfile, "%s\t%s\t%s\n", "#姓名", "移动电话", "家庭电话");
    }


    while (is_vcard) {

        const char* ps = vcard_text;
        const char* ps2 = vcard_text;
        size_t len = strlen(vcard_text);

        ps = memfind(vcard_text, f_name_title, len);

        if (ps != NULL) {
            // 搜索解码姓名
            ps2 = memfind(ps, "\n", len - (ps - vcard_text));
            string str(ps + strlen(f_name_title), ps2);
            strcpy(name, str.c_str());
            qp_decode(name);
            utf8_to_gb(name, name, strlen(name));

            fprintf(outfile, "%s\t", name);

            // 获得电话号码
            ps = memfind(vcard_text, mobile_number_title, len);
            if (ps != NULL) {
                ps2 = memfind(ps, "\n", len - (ps - vcard_text));
                string str(ps + strlen(mobile_number_title), ps2);
                strcpy(mobile_number, str.c_str());

                fprintf(outfile, "%s\t", mobile_number);

            }

            // 获得家庭电话
            ps = memfind(vcard_text, home_number_title, len);
            if (ps != NULL) {
                ps2 = memfind(ps, "\n", len - (ps - vcard_text));
                string str(ps + strlen(home_number_title), ps2);
                strcpy(home_number, str.c_str());

                fprintf(outfile, "%s", home_number);

            }

            fprintf(outfile, "%s", "\n");

        }


        // 移动到下一个 搜索区间
        count = file_size - offset ;
        vps = inbuf + offset;
        is_vcard = menSearch_vcard(vps, count, vcard_head, vcard_end,  vcard_text, &offset);

    }

    // 释放内存和关闭文件
    delete[]  vcard_text;
    delete[]  inbuf;
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



char str[] = "=E9=80=9A=E8=AE=AF=E5=BD=95=E9=80=9A=E8=AE=AF=E5=BD=95=E9=80=9A=E8=AE=AF=E5=BD=95";
qp_decode(str);
printf("%s\n", str);
utf8_to_gb(str, str, strlen(str));
printf("%s\n", str);

gb_to_utf8(str, str, strlen(str) * 3 / 2 + 1);  // len 要足够，不然不转换
printf("%s\n", str);


*/
#endif


void help()
{
    printf("本工具把通讯录 从VCARD格式导出文本TEXT电话号码  BY Hong Wenjun\n\n");
    printf("示例 1 ：D:\\>vcf2txt.exe  TelVcard.vcf \n");
    printf("示例 2 ：D:\\>vcf2txt.exe  TelVcard.vcf  TelText.txt\n");
    printf("\n输出文件不填，结果显示在屏幕上\a\n");

}

long get_file_size(FILE* file)
{
    long size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}

// 内存匹配函数memfind
char* memfind(const char* buf, const char* tofind, size_t len)
{
    size_t findlen = strlen(tofind);
    if (findlen > len) {
        return ((char*)NULL);
    }
    if (len < 1) {
        return ((char*)buf);
    }

    {
        const char* bufend = &buf[len - findlen + 1];
        const char* c = buf;
        for (; c < bufend; c++) {
            if (*c == *tofind) { // first letter matches
                if (!memcmp(c + 1, tofind + 1, findlen - 1)) { // found
                    return ((char*)c);
                }
            }
        }
    }

    return ((char*)NULL);
}

// 内存搜索VCARD 提取标记之间字符，装载到 vcard_text[]
bool menSearch_vcard(const char* vps, size_t count,
                     const char* flag_beg, const char* flag_end,
                     char* vcard_text, size_t* offset)
{
    char* pch = NULL;
    char* pch2 = NULL;
    pch = memfind(vps, flag_beg, count);
    if (pch == NULL) {
        //   printf("警告: 内存缓冲区找不到 flag_beg\n");
        return false;
    }
    if (pch != NULL) {
        pch2 = memfind(vps, flag_end, count);
        if (pch2 == NULL)
            return false;

        string str(pch , pch2 + strlen(flag_end));
        strcpy(vcard_text, str.c_str());

        *offset += pch2 - vps + strlen(flag_end); // 记录搜索结果的结尾偏移
    }


    return true;
}

