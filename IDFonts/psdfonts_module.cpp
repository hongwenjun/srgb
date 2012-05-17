#include "idtxt.h"
#include "psdfonts.h"


// 搜索PSD文件缓存，把字体PsName装载到容器
bool SearchFontSet_PsnameToMap(const char* buffer, size_t count, map<string, size_t>& flag_map)
{
    const char* flag_beg = "/FontSet [";
    const char* flag_end = "]";
    string strFontSet;

    // 搜索 /FontSet [ 区间保存到 strFontSet
    const char* ps = buffer;
    char* pch = NULL;
    char* pch2 = NULL;
    pch = memfind(ps, flag_beg , count);
    if (pch == NULL) {
        //   printf("警告: 内存缓冲区找不到 /FontSet\n");
        return false;
    }

    while (pch != NULL) {
        pch2 = memfind(pch, flag_end , 1024);
        if (pch2 != NULL) {
            string str(pch + strlen(flag_beg) , pch2);
            strFontSet += str;
        }
        ps = pch + strlen(flag_beg);
        pch = memfind(ps, flag_beg , count - (ps - buffer));
    }

    size_t pos = strFontSet.find('\0');
    while (pos != string::npos) {
        strFontSet.erase(pos, 1);  // 删除宽字节的 \0
        pos = strFontSet.find('\0');
    }

    flag_beg = "/Name (\xFE\xFF";
    flag_end = ")";
    count = strFontSet.size();
    buffer = strFontSet.c_str();
    // 从临时的 strFontSet 中搜索 /Name ( 区间保存到 提取PS字体名
    ps = buffer;
    pch = NULL;
    pch2 = NULL;
    pch = memfind(ps, flag_beg , count);
    while (pch != NULL) {
        pch2 = memfind(pch, flag_end , 1024);
        if (pch2 != NULL) {
            string str(pch + strlen(flag_beg) , pch2);
            ++flag_map[str]; // 装载到字符串容器
        }
        ps = pch + strlen(flag_beg);
        pch = memfind(ps, flag_beg , count - (ps - buffer));
    }

    {

        // 删除系统自带的超常用字体
        flag_map.erase("AdobeInvisFont");
        flag_map.erase("Arial-BoldMT");
        flag_map.erase("ArialMT");
        flag_map.erase("FangSong_GB2312");
        flag_map.erase("KaiTi_GB2312");
        flag_map.erase("NSimSun");
        flag_map.erase("SimHei"); // 黑体
        flag_map.erase("SimSun"); // 宋体
        flag_map.erase("TimesNewRomanPS-BoldMT");
        flag_map.erase("TimesNewRomanPSMT");
    }

    return true;
}
