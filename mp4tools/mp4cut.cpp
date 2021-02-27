#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>

void replace_colon(char* str)
{
    while (*str) {
        if (*str == ':')
            *str = '-';
        str++;
    }
}

int main(int argc, char* argv[])
{
    if (4 != argc) {
        puts("Usage: mp4cut.exe  sample.mp4  00:08  01:18 ");
        return -1;
    }

    /*******
    FFMPEG  按时间截取视频
    ffmpeg  -i ./plutopr.mp4   \
    -vcodec copy -acodec copy -ss 00:18:45 -to 00:19:36  \
     ./cutout1.mp4  -y
    *******/

    char cmdline[4096];
    sprintf(cmdline, "ffmpeg -i  \"%s\"  -vcodec copy -acodec copy -ss %s  -to %s  -y  \"%s\" ",
            argv[1], argv[2], argv[3], argv[1]);

    char newfile[512];    // 时间戳文件名后缀
    sprintf(newfile, ".Cut_%s_%s.mp4\" ", argv[2], argv[3]);
    replace_colon(newfile);

    char* pch = strrchr(cmdline, '.');

    FILE* pFile;
    pFile = fopen(argv[1], "r");
    if (pch != NULL) {
        strcpy(pch, newfile);

        if (pFile != NULL) {
            //  puts(cmdline);

            system(cmdline);
        }

    }
}
