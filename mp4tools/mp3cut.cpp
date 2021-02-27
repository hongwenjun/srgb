#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>

int main(int argc, char* argv[])
{
    if (5 != argc) {
        puts("Usage: mp3cut.exe  sample.mp3  00:08  01:18  Name.mp3 ");
        return -1;
    }

//  FFMPEG  按时间截取mp3音乐
//  ffmpeg  -i sample.mp3  -acodec copy -ss 00:18:45 -to 00:19:36  -y  name.mp3

    char cmdline[4096];
    sprintf(cmdline, "ffmpeg -i  \"%s\"  -acodec copy -ss %s  -to %s  -y  \"%s\" ",
            argv[1], argv[2], argv[3], argv[4]);

    FILE* pFile;
    pFile = fopen(argv[1], "r");
    if (pFile != NULL) {
        //  puts(cmdline);
        system(cmdline);
    }

}
