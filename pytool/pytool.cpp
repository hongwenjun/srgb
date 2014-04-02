#include "pytool.h"


static  wstring yb[]  = { L"āáǎàā", L"ōóǒòō", L"ēéěèē", L"īíǐìī", L"ūúǔùū", L"ǖǘǚǜǖ" };
static  wchar_t abc[]  = {L"aoeiuv"};
/** 将字母替换成拼音符号
 *  @param str 英文字母
 *  @return  拼音符号
 *
 * 汉语拼音中标声调位置的規則如下：
 * 如果有a，则标在a上。如果没有a，但有o或e，则标在这两个字母上。这两个字母不会同时出现。
 * 如果也没有o和e，则一定有i、u或ü。如果i和u同时出现，则标在第二个韵母上。这是特别针对ui和iu而言的（这两个音的实际读音应该是uei和iou）。如果i和u不同时出现，则标在出现的那个韵母上。
 * 声调一律标原调，不标变调。但是在语音教学时可以根据需要按变调标写。
 */
wstring& abc2py(wstring& str)
{
    // 特例iu ，实际读音 iou ，音标在u上
    bool iu_flag = false;
    size_t pos;
    if ((pos  = str.find(L"iu", 0)) != string::npos) {
        str[pos] =  L'|';   // | 转意 i
        iu_flag = true ;
    }

    for (size_t f = 0; f != wcslen(abc); f++) { //  aoeiuü 的优先级注解音标
        for (size_t i = 0; i < str.length(); i++) {
            if (abc[f] == str[i]) {
                size_t tone = str[str.size() - 1] - '0';   // 获取音标值 1-5
                if (tone != 0)
                    str[i] = yb[f][tone - 1];               // 替换aoeiuv为音标
                if (iu_flag) // i 替换回来
                    str[pos] =  L'i';
                str.erase(str.size() - 1, str.size());  // 删除原yb数字
                return str;
            }
        }
    }
    return str;
}

// 功能 获得当前路径
char* GetAppDir(char* szPath)
{
    char* ret = szPath;
    int i;
    GetModuleFileName(NULL, szPath, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    i = strlen(szPath);
    while ((i > 0) && (szPath[i - 1] != '\\')) { // 删除文件名，只留下目录
        szPath[--i] = 0;
    }
    return ret;
}


bool InitPinyindb(map<wchar_t, wstring>& pyTable)
{
    wchar_t hz;     // 宽字节的汉字  4E08 =>万
    wchar_t abcpy[64]; // 存放没转换的  (zhang4)
    wchar_t* ps = NULL;
    wstring pyyb; // 转换拼音音标

    char pinyindb[MAX_PATH];
    GetAppDir(pinyindb);
    strcat(pinyindb, "/pinyindb.dat");
    FILE* pFile = fopen(pinyindb, "r");

    // 把数据文件读进来装进拼音表，每次读入一个汉字
    if (pFile == NULL) {
        perror("读拼音表文件pinyindb.dat错误!!!\a\n");
        return false;
    }
    while (!feof(pFile)) {
        fwscanf(pFile, L"%x %s" , &hz , abcpy);

        ps = wcstok(abcpy, L"(,)");
        if (ps != NULL) {
            pyyb = ps;
            abc2py(pyyb);
        } else
            pyyb = L"";
        pyTable.insert(make_pair(hz , pyyb));
    }

    fclose(pFile);
    return true;
}

void AppHelp() // 帮助
{
    wprintf(
        L"本工具把文本中的汉字标注拼音，建立InDesign使用的标记文本\n"
        L"[版本 0.3] BY Hong Wenjun (蘭公子)  2012.2.21  \n"
        L"\n"
        L"示例 1 ：pytool.exe   输入文件.txt   \n"
        L"示例 2 ：pytool.exe   输入文件.txt   E:\\桌面\\拼音标记文件.txt  \n"
        L"示例 3 ：pytool.exe   \n"
        L"\n"
        L"使用说明: 1> 点 进入命令行快捷图标 进入命令行，输入 pytool 空格 \n"
        L"          2> 把你要输入的文件 拖入窗口，按回车，执行示例一。 \n"
        L"          3> 生成 默认文件 ID标记文件.txt \n"
        L"详细使用：见视频教程。使用时，请关闭 国内垃圾的监控软件!!! \n");
}
