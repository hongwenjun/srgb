#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
 
 
using namespace std;
 
// 删除字符串前后空白
string strTrim(string& str)
{
    str = str.substr(str.find_first_not_of("\t \n"));
    str = str.substr(0, str.find_last_not_of("\t \n") + 1);
    return str;
}
 
// 读取配置文件中的值
char* read_ini(char* filename, char* key, char* value)
{
    string line;
    fstream file(filename, fstream::in);
 
    while (getline(file, line)) {
 
        // 跳过没有'='号 和 有';' 的注解行
        if (line.find('=') == string::npos)
            continue ;
        line = strTrim(line);
        if (line[0] == ';')
            continue ;
 
        if (line.find(key) != string::npos) {
            line = line.substr(line.find('=') + 1);
            strcpy(value, line.c_str());
        }
 
    }
    file.close();
    return value;
}
 
 
int main()
{
    char filename[] = "my.ini";
    char key[512] = "computername";
    char value[512];
 
    cout << key  << "\t" <<  read_ini(filename, key, value) << endl;
 
    return 0;
}
