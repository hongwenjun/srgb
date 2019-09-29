#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_fileSize(const char* filename);
const char* GetFileBaseName(const char* szPath);
char* memfind(const char* buf, const char* tofind, size_t len);
void helpinfo(void);

int main(int argc, char* argv[])
{
    if (argc == 1) {
        helpinfo();
        return -1;
    }

    const char* filename = argv[1];
    const char* savefile = GetFileBaseName(filename);

    FILE* psdfile = fopen(filename, "rb");
    if (psdfile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    long filesize = get_fileSize(filename);
    size_t result;


    char* buf = new char[filesize];
    result = fread(buf, 1, filesize, psdfile);


    if (filesize > 10 * 1024 * 1024)
        printf("文件大小: %d MB\t\t文件名:%s\n",  filesize / 1024 / 1024, savefile);
    else
        printf("文件大小: %d KB\t\t文件名:%s\n",  filesize / 1024 + 1, savefile);

    fclose(psdfile);

    const char* flag_beg = "<photoshop:DocumentAncestors>" ;
    const char* flag_end = "</photoshop:DocumentAncestors>" ;

    char* pch = NULL;
    char* pch2 = NULL;
    size_t pos = 0;
    size_t pos2 = 0;
    bool flag_found = false;

    pch = memfind(buf, flag_beg, result);
    pch2 = memfind(buf, flag_end, result);

    while ((pch != NULL) && (pch2 != NULL)) {

        pos = pch - buf;
        pos2 = pch2 - buf + strlen(flag_end);

        if (pos2 < pos) {
            fputs("File error", stderr);
            exit(1);
        }

        printf("DocumentAncestors标记起止: 0x%X  -->  0x%X\n",  pos, pos2);

        memset(pch, ' ', pos2 - pos);   // 清除垃圾数据，使用空格填充

        flag_found = true;

        pch = memfind(buf + pos2, flag_beg, result - pos2);
        pch2 = memfind(buf + pos2, flag_end, result - pos2);
    }

    if (!flag_found) {
        fputs("PSD文件没有 DocumentAncestors标记，不需要工具 ", stderr);
        exit(1);
    }

    // 默认修复文件名，前缀 Fix_
    char fix_name[256];
    if (argc == 2) {
        strcpy(fix_name, "Fix_");
        strcat(fix_name, savefile);
        savefile = fix_name;
    }

    if (argc == 3) {
        strcpy(fix_name, argv[2]);
        strcat(fix_name, "\\");
        strcat(fix_name, savefile);
        savefile = fix_name;
    }


    FILE* pFile = fopen(savefile, "wb");
    fwrite(buf, sizeof(char), result, pFile);

    printf("清理垃圾后文件名:\t%s\n记得要用PS打开保存才能把文件变小!\n", savefile);

    delete [] buf;
    fclose(pFile);


    return 0;
}




void helpinfo(void)
{
    printf("本工具清除PSD文件中垃圾数据 by 蘭公子 sRGB.vicp.net\n");
    printf("开源代码    https://github.com/hongwenjun/psdda\n\n");
    printf("Usage: psdda.exe  test.psd  [SaveFilePath] \n\n");
    printf("可以 只一个文件名参数,自动加前缀Fix_; 指定保存目录，副本同名!\n\n");
}


// 获得文件大小
size_t get_fileSize(const char* filename)
{
    FILE* pfile = fopen(filename, "rb");
    fseek(pfile, 0, SEEK_END);
    size_t size = ftell(pfile);
    fclose(pfile);
    return size;

}


// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath)
{
    const char* ret = szPath + strlen(szPath);
    while (!((*ret == '\\') || (*ret == '/'))
            && (ret != (szPath - 1))) // 得到文件名
        ret--;
    ret++;
    return ret;
}

// 内存查找字符 memfind
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
