#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>

int main(int argc, char* argv[])
{
    if (1 == argc) {
        puts("Usage: mp4Tomp3.exe  sample.mp4 ");
        return -1;
    }

    char cmdline[4096];
    strcpy(cmdline, "ffmpeg -i  \"");
    strcat(cmdline, argv[1]);
    strcat(cmdline, "\"   -ar  44100 -ac 2 -ab 192k -f mp3  \"");
    strcat(cmdline, argv[1]);

    char* pch = strrchr(cmdline, '.');

    if (pch != NULL) {

        strcpy(pch, ".mp3\" ");

        FILE* pFile;
        pFile = fopen(argv[1], "r");
        if (pFile != NULL) {

            //   puts(cmdline);

            system(cmdline);
        }

    }
}
