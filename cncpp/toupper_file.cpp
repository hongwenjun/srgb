// 修改文件示例  C++ 修改文件中单词大小写（2011-07-14 11:49）
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
using namespace std;

// 修改英文单词第一个字母大写
void first_toupper(char * sp)
{

    if (islower(*sp))
        *sp = toupper(*sp);
    while (*sp) {
        if (isspace(*sp) || ispunct(*sp)) { // 如果参数是空格类字符（即：单空格，制表符，垂直制表符，满页符，回车符，新行符），
            // 或者参数是除字母，数字和空格外可打印字符
            sp++;
            *sp = toupper(*sp);
        } else
            sp++;

    }
}

void all_toupper(char * sp)
{
    while (*sp) {
        *sp = toupper(*sp);
        sp++;
    }
}


int main()
{

    char s[40] = "hello china!";
    first_toupper(s);         // 测试函数
    cout << s << endl;
    all_toupper(s);         // 测试函数
    cout << s << endl;

    char * buffer;
    long size;

    fstream iofile("toupper_file.cpp", ifstream::binary | fstream::in | fstream::out);     // 二进制打开文件输入

    // 取得文件大小
    iofile.seekg(0, ifstream::end);  // 输入流定位到文件末位  函数seekg()用于输入流
    size = iofile.tellg();           // tellg()函数用于输入流，并返回流中"get"指针的当前位置
    iofile.seekg(0);                 // 重新设置输入流到文件头

    cout << "test.txt 文件大小: " << size << "字节" << endl;

    // 为文件内容分配内存(缓冲区)
    buffer = new char [size];

    // 读取文件输入流到缓冲区(读文件)
    iofile.read(buffer, size);

    iofile.seekg(0);   // 移到文件头
    all_toupper(buffer);  // 修改缓冲区

    // 缓冲区写到文件输出流(写文件)
    iofile.write(buffer, size);

    // 释放动态分配的内存  (释放缓冲)
    delete[] buffer;

    iofile.close();
    return 0;
}
