#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <map>

#include "CDirectoryExplorer.h"
#include "bdgps_api.h"

using namespace std;
using namespace mrpt::system;


FILE* out_google_maps;
GPS_FILEHEAD  first_file_gps_filehead;
char first_filename[512];

int readgpsfile(const char* filename, map<time_t, GPS_POINT>& map_gps_point);
bool make_gpsfile(FILE* bin_file, const char* filename);
// ------------------------------------------------------
//                        MAIN
// ------------------------------------------------------
int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            printf("https://github.com/hongwenjun/TrajectoryCombine   <BY Hong Wenjun> 2016.3.4\n\n");
            printf("Usage: TrajectoryCombine.exe  test.bin.gz  [test2.bin.gz ...]\n");
            return -1;
        }

        map<time_t, GPS_POINT> map_gps_point;

        for (int i = 1; i < argc; i++) {
            CDirectoryExplorer::TFileInfoList lst;
            CDirectoryExplorer::explore(argv[i], FILE_ATTRIB_ARCHIVE, lst);

            if (lst.empty()) {
                cerr << "Error: Not found " << argv[i] << endl;
                continue;
            }

            for (CDirectoryExplorer::TFileInfoList::const_iterator it = lst.begin(); it != lst.end(); it++) {
                if (i == 1 && it == lst.begin())
                    strcpy(first_filename, it->wholePath.c_str()) ;  // 读取第一个文件名

                readgpsfile(it->wholePath.c_str(), map_gps_point);   // 读取所有的 gps节点到容器

            }
        }

        FILE* bin_file = tmpfile();
        for (auto it = map_gps_point.begin(); it != map_gps_point.end(); ++it) {
            fwrite(&it->second,  sizeof(GPS_POINT), 1, bin_file);    // 写容器里的gps节点到bin文件
        }

        strcat(first_filename, ".NewCombine.bin.gz");
        if (make_gpsfile(bin_file, first_filename))
            cerr << "\nTrajectory Combine New File:\n" << first_filename << endl;
        fclose(bin_file);



        return 0;

    } catch (exception& e) {
        cerr << "EXCEPCTION: " << e.what() << endl;
        return -1;
    } catch (...) {
        cerr << "Untyped excepcion!!";
        return -1;
    }
}


bool make_gpsfile(FILE* bin_file, const char* filename)
{
    // 获得bin数据大小
    rewind(bin_file);
    fseek(bin_file, 0, SEEK_END);
    size_t bin_size = ftell(bin_file);
    rewind(bin_file);

    // 建立gps文件头
    GPS_FILEHEAD* gps_filehead = new GPS_FILEHEAD;
    size_t gps_head_size = sizeof(GPS_FILEHEAD);

    memcpy(gps_filehead, &first_file_gps_filehead, sizeof(GPS_FILEHEAD));
    gps_filehead->data_pos = 0x18;
    gps_filehead->data_ver = 0x06;    // 06 是06版轨迹文件，之前是04  05



    // 分配gps文件头 加  bin数据大小 内存
    size_t buf_size =  gps_head_size + bin_size;
    char* buffer = new char[buf_size];

    // 加载到缓存
    memcpy(buffer, gps_filehead, gps_head_size);
    fread(buffer + gps_head_size,  bin_size, 1, bin_file);


//    // 写文件  测试用bin文件.bin
//    FILE* bf = fopen("测试用bin文件.bin" , "wb");
//    fwrite(buffer, buf_size, 1, bf);
//    fclose(bf);
//    cerr <<  buf_size << endl;

    // 写入gz压缩的百度导航轨迹文件
    gzFile gzf = gzopen(filename, "rw");
    if (gzwrite(gzf, buffer,  buf_size)  < 0)
        return false;
    gzclose(gzf);

    delete[] buffer;
    delete gps_filehead;
    return true;
}

int readgpsfile(const char* filename, map<time_t, GPS_POINT>& map_gps_point)
{
    FILE* pFile = fopen(filename, "rb");
    if (!pFile) {
        cerr << "Error: Cannot open" << filename << endl;
        return -1;
    }


    size_t data_size = get_fileSize(filename);
    int32_t gz_header_3byte = GZ_HEADER_3BYTE;
    fread(&gz_header_3byte, 1, sizeof(gz_header_3byte), pFile);
    rewind(pFile);

    // 判断是否是gz文件
    bool is_gzfile = (GZ_HEADER_3BYTE == (gz_header_3byte & 0xFFFFFF));

    char* buffer = NULL;
    if (is_gzfile) {
        // 读取gz文件，解压bdgps轨迹文件到内存
        data_size = get_gzbinSize(filename);

        gzFile gzf = gzopen(filename, "rb");
        buffer = new char[data_size + 1];
        buffer[data_size] = 0;

        if (gzread(gzf, buffer,  data_size)  < 0)
            return -1;
        gzclose(gzf);
    } else {
        cerr << "Error: NO GPS_Trajectory in " << filename << endl;
        return -2;
    }

    // 分析内存中的数据
    char* gps_buffer = buffer;
    GPS_FILEHEAD* gps_filehead = (GPS_FILEHEAD*)gps_buffer;   // bin文件头

    // 屏蔽  xxxx_rp.bin.gz 非轨迹文件
    if (!((gps_filehead->empty_1 == 0x00) && (gps_filehead->data_pos == 0x18))) {

        cerr << "Error: NO GPS_Trajectory in " << filename << endl;
        return -2;
    }


    if (strcmp(filename, first_filename) == 0) {
        // 获取第一个文件的文件头
        memcpy(&first_file_gps_filehead, gps_filehead, sizeof(GPS_FILEHEAD));

        //      printf("\n\n%s\n%s\n %d", filename, first_filename, first_file_gps_filehead.unknown2);
    }


    // 兼容旧版本 02 04 05 和当前 06版本数据，使用指针回退，兼容数据结构不一样长
    int ver_offset = 0;
    if (gps_filehead->data_ver == 5) {
        ver_offset = sizeof(int32_t);
    } else if (gps_filehead->data_ver <= 4) {
        ver_offset = 2 * sizeof(int32_t);
    }


    GPS_POINT* gps_point = (GPS_POINT*)(gps_buffer + gps_filehead->data_pos);  // 第一条GPS记录
    int gps_point_total = (data_size - gps_filehead->data_pos) / (sizeof(GPS_POINT) - ver_offset); // GPS记录条目数

    int fraction = 10;   // 默认输出(1/10) GPS节点
    int count = fraction;
    while (gps_point_total--) {
        if (count % fraction == 0) {
            map_gps_point.insert(make_pair(gps_point->timestamp, *gps_point));
            count--;
        } else if ((--count) == 0)
            count = fraction;

        gps_point++;
        gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);    // 兼容旧版本 02 04 05 当作 06版本数据读，读好来个指针回退
    }

    gps_point = (GPS_POINT*)((char*)gps_point + ver_offset);
    --gps_point;
    map_gps_point.insert(make_pair(gps_point->timestamp, *gps_point));

    // 释放文件和内存
    fclose(pFile);
    delete[] buffer;
    return 0;
}
