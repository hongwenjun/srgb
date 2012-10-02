 #include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <stdio.h>
#include <string.h>

using namespace std;
// StudentID  Score1  Score2  Score3  average
struct Student {
    char StudentID[16];
    int Score1 , Score2 , Score3  ;
    double average;
};

// 加载学生成绩库
void LoadStudentLib(istream& fs , vector<Student>& vs);
// 打印学生成绩表
void PrintStudentList(vector<Student>& vs);

// 比较函数
bool IDCmp(const Student& a , const Student& b) {return (strcmp(a.StudentID, b.StudentID)  < 0);}
bool averageCmp(const Student& a , const Student& b) { return a.average > b.average;}

FILE* CerrLog = fopen("错误数据.log" , "wb+");
void PrintCerrLog();  // 输出错误日志文件

int main()
{
    vector<Student> verStudent;

    // cin 换成文件 就可以直接文件输入了，用cin要手工ctrl+z 结束输入
    // LoadStudentLib(cin , verStudent);

    ifstream file("Student.txt");
    LoadStudentLib(file , verStudent);
    file.close();

    vector<Student>::iterator first = verStudent.begin();
    vector<Student>::iterator last = verStudent.end();

    cout << string(78 , '-') << endl;
    sort(first , last, IDCmp);   // 按学号排序
    PrintStudentList(verStudent);

    cout << string(78 , '-') << endl;
    sort(first , last, [](const Student & a , const Student & b)
                    { return a.average > b.average; } ); // 按平局分排序 也可以使用

    PrintStudentList(verStudent);

    cout << string(78 , '-') << endl;
    PrintCerrLog();  // 打印错误数据日志
    cout << string(78 , '-') << endl;
    return 0;
}


void LoadStudentLib(istream& fs , vector<Student>& vs)
{
    char ID[16] = {0};
    int S1 , S2 , S3  ;
    double Avg;
    Student tmp;
    string line;

    // 匹配正则公式 \s*\S+\s+(\d+\s+\d+\s+\d+)\s*   [ 朱佩誉 79 122 70 ]
    string rs = "\\s*\\S+\\s+(\\d+\\s+\\d+\\s+\\d+)\\s*";
    regex expression(rs); // 字符串传递给构造函数，建立正则表达式

    while (getline(fs, line)) {
        if (line[0] == '#'  || !regex_match(line, expression)) {  // #开头是注解行和无效数据写进错误记录
            fprintf(CerrLog, "无效line-> %s\n" , line.c_str());
            continue;
        }

        sscanf(line.c_str(), "%s %d %d %d" , ID, &S1 , &S2 , &S3);
        Avg = (S1 + S2 + S3) / 3.0;
        strcpy(tmp.StudentID , ID);
        tmp.Score1 = S1;  tmp.Score2 = S2;
        tmp.Score3 = S3;  tmp.average = Avg;
        vs.push_back(tmp);
    }
}

void PrintStudentList(vector<Student>& vs)
{
    vector<Student>::iterator it = vs.begin();
    for (; it != vs.end() ; ++it)
        printf("%s\t%d\t%d\t%d\t%0.2f\n" , it->StudentID ,
               it->Score1 , it->Score2 , it->Score3 , it->average);
}

void PrintCerrLog()  // 输出错误日志文件
{
    size_t fsize =  ftell(CerrLog);     // 获得文件大小
    char* pfbuf = new char[fsize + 1];  // 分配内存
    pfbuf[fsize] = '\0';                // 缓冲区末位清零

    fseek(CerrLog, 0, SEEK_SET);        // 移动日志文件头部
    fread(pfbuf, 1 , fsize, CerrLog);   // 读取日志到缓冲区
    printf("%s", pfbuf);                // 打印日志(缓冲区中)
    delete[] pfbuf;                     // 回收内存
}

/* 输入文件 Student.txt
#姓名 成绩1 成绩2 成绩3
杨丰艳   81  121     71
董颖  77  118.8     68
#王乃东   86  119     73
            朱义城   81  127     72
李超  7B  128     71
李江卫80  122     71
廉功乙   80  121  x   70
邓玉姝 79  120     69
胡鑫莹   90  136     79
王文涛   76  141     73
韩佳彤   81  126     72
董晓明   91  146     82
傅瀛  86  128     75
郝玉靖   85  128     74
高 珊  82  126     73
阿茹罕   80  125     71
马玉玲   79  127     71
朱佩誉   79  122     70
           周春苗   77  119     68
   #张丽娅   87  130     76
石荣广   81  127     72
沈 泊  78  129     71
何力威   86  135     77
黄   悦  82  133     74
谢飞  75  130     70
张爱梅   85  126     74
李鸥  80  126     71
刘立孟   80  126     71
麻艳伟   78  126     70
刘蒙  66  126     65
申小敏   87  135     77
张慧超   83  132     75
田洋洋   82  129     73
赵华军   77  137     73
吴昉  80  128     72
刘光宗   78  126     70
武冰  77  131     71
王中君   78  126     70
何莹  75  126     69
刘喜成   87  133     77
张良宝   77  137     73
郑勇维   79  128     72
李云晴   78  128     71
李霁  80  121     70
陈金花   75  110     65
朝泽东   88  132     77
李进  85  131     75
王佳  82  126     73
路磊  79  118     69
唐智伟   77  115     67
富瑶  78  109     66
王莹  72  109     63
*/
