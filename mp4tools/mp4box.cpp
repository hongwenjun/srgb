#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>


int main(int argc, char* argv[])
{
    if (3 > argc) {
        puts("Usage: mp4box.exe  1.mp4  2.mp4  3.mp4 ");
        return -1;
    }

    FILE* filelist = fopen("filelist.txt", "w+");
    for (int i = 1 ; i != argc ; i++) {
        fprintf(filelist, "file  '%s'\n", argv[i]);
    }
    fclose(filelist);

/// ##  ffmpeg合并多个mp4视频
/// ffmpeg -f concat -i filelist.txt -c copy output_set.mp4

    char cmdline[] = "ffmpeg -f concat -i filelist.txt -c copy output_set.mp4 -y";
    system(cmdline);

    printf("%\n%s\n", cmdline);

}
