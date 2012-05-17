#include "idtxt.h"

// 获得文件长度
size_t get_file_Size(fstream& file)
{
    file.seekg(0, ios::end);
    size_t length = file.tellg();
    file.seekg(0, ios::beg);
    return length;
}

// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath)
{
    const char* ret = szPath + strlen(szPath);
    while ((*ret != '\\')&&(ret != (szPath-1)))  // 得到文件名
        ret--;
    ret++;
    return ret;
}


// 内存匹配函数memfind
char* memfind(const char* buf, const char* tofind, size_t len)
{
    size_t findlen = strlen(tofind);
    if (findlen > len) {
        return((char*)NULL);
    }
    if (len < 1) {
        return((char*)buf);
    }

    {
        const char* bufend = &buf[len - findlen + 1];
        const char* c = buf;
        for (; c < bufend; c++) {
            if (*c == *tofind) { // first letter matches
                if (!memcmp(c + 1, tofind + 1, findlen - 1)) { // found
                    return((char*)c);
                }
            }
        }
    }

    return((char*)NULL);
}

// 内存搜索，提取标记之间字符，装载到字符串容器
bool menSearch_FlagToMap(const char* buffer, size_t count,
                         const char* flag_beg, const char* flag_end,
                         map<string, size_t> &flag_map)
{
    const char* ps = buffer;
    char* pch = NULL;
    char* pch2 = NULL;
    pch = memfind(ps, flag_beg , count);
    if (pch == NULL) {
     //   printf("警告: 内存缓冲区找不到 flag_beg\n");
        return false;
    }
    while (pch != NULL) {
        pch2 = memfind(pch, flag_end , 1024);
        if (pch2 != NULL) {
            string str(pch + strlen(flag_beg) , pch2);
            {
                // Utf-8 转 GBK string版本，按需求是否屏蔽改语句
                ConvertUtf8ToGBK(str);
            }
            ++flag_map[str]; // 装载到字符串容器
        }
        ps = pch + strlen(flag_beg);
        pch = memfind(ps, flag_beg , count - (ps - buffer));
    }
    return true;
}

//  Utf-8 转 GBK  C风格字符串版本
char* ConvertUtf8ToGBK(char* strUtf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
    WCHAR* wszGBK = new WCHAR[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);

    strcpy(strUtf8 , szGBK);
    delete[] szGBK;
    delete[] wszGBK;
    return strUtf8;
}
//  Utf-8 转 GBK  string版本包装C风格字符串版本
string& ConvertUtf8ToGBK(string& strUtf8)
{
    char* psUTF8ToGBK = new char[strUtf8.size() + 1];
    strcpy(psUTF8ToGBK, strUtf8.c_str());
    strUtf8 = ConvertUtf8ToGBK(psUTF8ToGBK);
    delete[] psUTF8ToGBK;
    return strUtf8;
}


#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 1024
// 枚举搜索子键 名称和数值
void QuerySubKeyValues(HKEY hKey , map<string, string> &map_SystemFonts)
{

    DWORD    cValues;                   // number of values for key
    // Get the class name and the value count.   获取类的名称和值数

    RegQueryInfoKey(hKey, NULL, NULL, NULL,  // 只想获取 名称的计数，所以把其他的都NULL
                    NULL, NULL, NULL, &cValues,  // 不知道会不会有大问题
                    NULL, NULL, NULL, NULL);

    // Enumerate the key values.   枚举关键值
    char tcKeyName[MAX_VALUE_NAME] = {};                // 注册表名称值
    DWORD dwKeyNameLen = MAX_VALUE_NAME;
    BYTE* pbbinKeyData = new BYTE[MAX_VALUE_NAME];      // 注册表数据值
    DWORD dwKeyDataLength = MAX_VALUE_NAME;
    if (cValues) {
//        printf("Number of values: %ld\n", cValues);

        for (size_t i = 0, retCode = ERROR_SUCCESS; i != cValues; i++) {
            dwKeyNameLen = MAX_VALUE_NAME;
            dwKeyDataLength = MAX_VALUE_NAME;

// 使用     RegEnumValue(hKey, dwIndex, tcKeyName, &dwKeyNameLen,
// 示例                  NULL, &dwKeyType, pbbinKeyData, &dwKeyDataLength);
            retCode = RegEnumValue(hKey, i,     // 子键 和 索引
                                   tcKeyName, &dwKeyNameLen,   // 名称值 和 长度
                                   NULL, NULL,
                                   pbbinKeyData, &dwKeyDataLength); // 数值 和 长度

            if (retCode == ERROR_SUCCESS) {
//              printf("(%d)\t%s\t=\t%s\n", i + 1, tcKeyName , pbbinKeyData); // 调试显示
                // 字库表装入容器
                map_SystemFonts.insert(make_pair(string(tcKeyName),
                                                 string((char*)pbbinKeyData)));
            }
        }
    }
    delete[] pbbinKeyData;
}

// 加载系统字体表到字体容器
bool LoadSystemFonts(map<string, string> &map_SystemFonts)
{
    // 根键、子键名称、和到子键的句柄
    HKEY hRoot = HKEY_LOCAL_MACHINE;
    const char* szSubKey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY hKey;

    // 打开指定子键，枚举搜索子键 名称和数值
    if (RegOpenKeyEx(hRoot, szSubKey, 0,  KEY_READ, &hKey) == ERROR_SUCCESS) {
        QuerySubKeyValues(hKey , map_SystemFonts);   // 字库表装入容器
    } else {
        printf("警告: 加载系统字体表失败\n");
        return false;
    }


    RegCloseKey(hKey);

    return true;
}

// windows控制台彩色文本函数
bool SetConsoleColor(WORD Color)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == 0)
        return false;
    BOOL ret = SetConsoleTextAttribute(handle, Color);
    return(ret == TRUE);
}
