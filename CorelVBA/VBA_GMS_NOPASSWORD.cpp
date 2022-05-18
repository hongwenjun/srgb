#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

size_t get_fileSize(const char* filename);
const char* GetFileBaseName(const char* szPath);
char* memfind(const char* buf, const char* tofind, size_t len);
void helpinfo(void);
int  GetFilePath(HWND hWnd, char* szFile);

int main(int argc, char* argv[])
{
    const char* filename = argv[1];
    if (argc == 1) {
        char* fbuf = new char[1024];
        GetFilePath(0, fbuf);
        filename = fbuf;
        fputs(filename, stderr);
    }
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

    const char* flag = "CMG=" ;
    const char* fix_flag = "CMG." ;

    char* pch = NULL;
    bool flag_found = false;

    pch = memfind(buf, flag, result);


    if (pch != NULL) {
        memcpy(pch, fix_flag, 4);
        flag_found = true;
    }

    flag = "DPB=" ;
    fix_flag = "DPB." ;

    pch = NULL;
    pch = memfind(buf, flag, result);

    if (pch != NULL) {
        memcpy(pch, fix_flag, 4);
        flag_found = true;
    }

    if (!flag_found) {
        fputs("GMS文件可能不用破解!", stderr);
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

    delete [] buf;
    fclose(pFile);
    return 0;
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


// 选择一个文件
int   GetFilePath(HWND hWnd, char* szFile)
{

    OPENFILENAME ofn;       // common dialog box structure
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 260; // 本来sizeof(szFile);
    ofn.lpstrFilter = "CorelVBA GMS文件(*.gms)\0*.gms\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    GetOpenFileName(&ofn);
    return   lstrlen(szFile);
}
