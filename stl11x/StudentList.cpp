#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace std;
// StudentID  Score1  Score2  Score3  average
struct Student {
    char StudentID[16];
    int Score1 , Score2 , Score3  ;
    double average;
};

void LoadStudentLib(istream& fs , vector<Student>& vs);
void PrintStudentList(vector<Student>& vs);

// 比较函数
bool IDCmp(const Student& a , const Student& b) {return (strcmp(a.StudentID, b.StudentID)  < 0);}
bool averageCmp(const Student& a , const Student& b) { return a.average > b.average;}

int main()
{
    vector<Student> verStudent;

    // cin 换成文件 就可以直接文件输入了，用cin要手工ctrl+z 结束输入
    LoadStudentLib(cin , verStudent);

    vector<Student>::iterator first = verStudent.begin();
    vector<Student>::iterator last = verStudent.end();

    cout << string(78 , '-') << endl;
    sort(first , last, IDCmp);   // 按学号排序
    PrintStudentList(verStudent);

    cout << string(78 , '-') << endl;
    sort(first , last, averageCmp); // 按平局分排序
    PrintStudentList(verStudent);

    return 0;
}



void LoadStudentLib(istream& fs , vector<Student>& vs)
{
    char ID[16] = {0};
    int S1 , S2 , S3  ;
    double Avg;
    Student tmp;
    string line;
    while (getline(fs, line)) {
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