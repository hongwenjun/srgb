#include <iostream>
#include <stdio.h>
#include <windows.h>
using namespace std;

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


int main()
{

    char* vcard_head = "BEGIN:VCARD\nVERSION:2.1\n";
    char* vcard_end = "END:VCARD\n";

    char* name_title = "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;";
    char* end_name = ";;;";

    char* f_name_title = "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";

    char*  mobile_number_title = "TEL;CELL:";
    char*  home_number_title = "TEL;HOME:";


    char name[512] = "通讯录";
    char mobile_number[16] = "1839593888";
    char home_number[16] = "610616"; // 可以存移动短号



    /*     把name 转换成 utf-8 ，然后 使用=EE=FF=EE取16进制进行编码 */
    gbk_to_utf8(name);
    utf_encode_hex(name);



    cout << vcard_head
         << name_title << name << end_name  << endl
         << f_name_title << name << endl
         << mobile_number_title << mobile_number << endl
         << home_number_title  << home_number << endl
         << vcard_end << endl;
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
