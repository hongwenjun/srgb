#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cctype>

using namespace std;
void help(); // 调用使用帮助
bool blank_lines(const string& str); // 检测空行
string& find_replace(string& str_line, const string& str1 , const string& str2); // 查找替换字符窜
string& Chinese_Punctuation_Conver_English(string& str_line); // 中文标点符号为英文

int main(int argc, char* argv[])
{
    if ((1 == argc)) {    //错误输入处理
        help();
        return -1;
    }
    ifstream infile(argv[1]);
    if (!infile.good()) {
        printf("文件错误：不能打开输入文件: %s \n\a\n", argv[1]);
        help();
        return -1;
    }
    // 主模块功能：中文标点符号为英文，按条件删除空行
    stringstream oss;   // 输出先写 oss 缓冲区
    string str_line;
    bool CodeBlocks_flag = false;
    while (getline(infile, str_line)) {
        Chinese_Punctuation_Conver_English(str_line);
        if (!blank_lines(str_line))   // 检测不是空行
            oss << str_line << endl;
        else if (CodeBlocks_flag) {
            oss << str_line << endl;
            CodeBlocks_flag = false;
        }
        // 如果当前语句块结束，保留下一行空行标记
        if (str_line[str_line.size() - 1] == '}')
            CodeBlocks_flag = true;
    }
    infile.close();
    ofstream outfile;
    if (2 < argc)
        outfile.open(argv[2]); // 输出到新文件
    else
        outfile.open(argv[1]); // 替换源文件
    outfile << oss.str();  // 缓冲区写文件
    cout << "本工具整理代码中的中文标点符号为英文，删除空行   BY Hong Wenjun\n\n"
         "Chinese Punctuation Conver English Punctuation OK! ...." << endl;
    return 0;
}

void help()
{
    printf("本工具整理代码中的中文标点符号为英文，删除空行   BY Hong Wenjun\n\n"
           "CodeTool filename [输出filename] \n\n"
           "示例 1 ：D:\\>CodeTool   D:\\app.cpp  \n"
           "示例 1 ：D:\\>CodeTool   D:\\app.cpp  D:\\app_new.cpp \n"
           "\n输出文件不填，覆盖原输入文件\n");
}


bool blank_lines(const string& str)
{
    string::const_iterator it;
    for (it=str.begin() ; it != str.end(); it++ ) {
        if (!isspace(*it)) // 如果不是空格，就不是空行
            return false;
    }
    return true;
}

string& find_replace(string& str_line, const string& str1 , const string& str2)
{
    string::size_type pos;
    size_t CPsize = str1.size();

    pos = str_line.find(str1); // 查找 str1 替换成 str2
    while (pos != string::npos) {
        str_line.replace(pos, CPsize, str2);
        pos = str_line.find(str1, pos + 1);
    }
    return str_line;
}

string& Chinese_Punctuation_Conver_English(string& str_line)
{
    find_replace(str_line, "“", "\"");
    find_replace(str_line, "”", "\"");

    find_replace(str_line, "‘", "\'");
    find_replace(str_line, "’", "\'");

    find_replace(str_line, "，", ",");
    find_replace(str_line, "。", ".");

    find_replace(str_line, "！", "!");
    find_replace(str_line, "？", "?");

    find_replace(str_line, "；", ";");
    find_replace(str_line, "：", ":");

    find_replace(str_line, "　", " "); // 全角空格
    find_replace(str_line, "（", "(");
    find_replace(str_line, "）", ")");
    find_replace(str_line, "〈", "<");
    find_replace(str_line, "〉", ">");

    return str_line;
}
