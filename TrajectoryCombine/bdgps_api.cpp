#include <bdgps_api.h>


// DEBUG 调试数据用
void debug_gps_point(FILE* outfile, GPS_POINT* gps_point)
{

    fprintf(outfile, "%d\t", gps_point->longitude);
    fprintf(outfile, "%d\t", gps_point->latitude);
    fprintf(outfile, "%d\t", gps_point->speed);
    fprintf(outfile, "%d\t", gps_point->bearing);
    fprintf(outfile, "%d\t", gps_point->accuracy);
    fprintf(outfile, "%d\t", gps_point->timestamp);

    fprintf(outfile, "%d\t", gps_point->unknown_1);
    fprintf(outfile, "%d\t", gps_point->unknown_2);
    fprintf(outfile, "%d\t", gps_point->unknown_3);
    fprintf(outfile, "%d\t", gps_point->unknown_5);
    fprintf(outfile, "%d\t", gps_point->unknown_6);
    fprintf(outfile, "%d\t", gps_point->unknown_7);
    fprintf(outfile, "%d\t", gps_point->unknown_8);
    fprintf(outfile, "%d\t", gps_point->unknown_9);

    fprintf(outfile, "%d\t", gps_point->unknown_ver05);
    fprintf(outfile, "%d\n\n", gps_point->unknown_ver06);

}


extern FILE* out_google_maps;

void print_gps_point(FILE* outfile, GPS_POINT* gps_point)
{

    fprintf(outfile, "%.5f\t", (float)gps_point->latitude / 100000);
    fprintf(outfile, "%.5f\t", (float)gps_point->longitude / 100000);
    fprintf(outfile, "%.1f\t", (float)gps_point->speed / 100 * 3600 / 1000);

    time_t rawtime = gps_point->timestamp;
    struct tm* timeinfo;
    char time_str [80];
    timeinfo = localtime(&rawtime);
    strftime(time_str, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    fprintf(outfile, "%s\n", time_str);

#if(0)
    debug_gps_point(outfile, gps_point);
#endif

    // 打印纬度和精度到google_maps用的html文件里
    fprintf(out_google_maps, "%.5f", (float)gps_point->latitude / 100000);
    fprintf(out_google_maps, "%%09%.5f/", (float)gps_point->longitude / 100000);

}

void help()
{
    printf("百度轨迹GPS: 轨迹记录文件导出Gps点(for Google地图版) BY Hong Wenjun 2016.3.4\n"
           "本软件源码 https://github.com/hongwenjun/TrajectoryCombine\n\n");
    printf("示例1: D:\\>bdgps.exe  test.bin [或者 test.bin.gz]  \n");
    printf("示例2: D:\\>bdgps.exe  test.bin.gz  gps.txt \n");
    printf("示例3: D:\\>bdgps.exe  test.bin.gz  gps.txt  -ALL \n");
    printf("示例4: D:\\>bdgps.exe  test.bin.gz  gps.txt  -60 \n");
    printf("\n输出文件不填，结果显示在屏幕上\a  <用-ALL参数所有Gps点，-60近每分钟频率>\n");
    printf("另外输出文件index.html, 用浏览器Gps点显示在Google地图上\n");

}


// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName)
{
    WIN32_FIND_DATA fd = {0};
    HANDLE hFind = FindFirstFile(lpFileName, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    }
    return ((hFind != INVALID_HANDLE_VALUE) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
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

// 获取gz文件的数据大小
size_t get_gzbinSize(const char* filename)
{
    const int BUFSIZE = 1024 * 1024;
    char* buf = new char[BUFSIZE];
    int data_size = 0;  int cnt = 0;
    gzFile gzf = gzopen(filename, "rb");
    while ((cnt = gzread(gzf, buf,   BUFSIZE))  > 0)
        data_size += cnt;

    gzclose(gzf);
    delete[] buf;
    return data_size;

}
// 功能 获得当前路径
char* GetAppDir(char* szPath)
{
    char* ret = szPath;
    GetModuleFileName(NULL, szPath, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    *(strrchr(szPath, '\\')) = '\0';    // 删除文件名，只留下目录
    return ret;
}

// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath)
{
    const char* ret = szPath + strlen(szPath);
    while ((*ret != '\\') && (ret != (szPath - 1))) // 得到文件名
        ret--;
    ret++;
    return ret;
}


