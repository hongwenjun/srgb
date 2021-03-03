#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>

int main(int argc, char* argv[])
{
    if (1 == argc) {
        puts("Usage: mp4Tomp3.exe  sample.mp4  [192k]");
        return -1;
    }

    const char* bitrate = "192k";
    if (3 == argc)
        bitrate = argv[2];

    char cmdline[4096];

    sprintf(cmdline, "ffmpeg -i \"%s\"  -ar 44100 -ac 2 -ab %s -f mp3  \"", argv[1], bitrate);
    strcat(cmdline, argv[1]);

    char* pch = strrchr(cmdline, '.');

    if (pch != NULL) {

        strcpy(pch, ".mp3\" ");

        FILE* pFile;
        pFile = fopen(argv[1], "r");
        if (pFile != NULL) {

            system(cmdline);
            puts(cmdline);
        }
    }
}
