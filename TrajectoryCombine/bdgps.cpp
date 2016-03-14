#include <bdgps_api.h>

FILE* out_google_maps;


int main(int argc, char* argv[])
{

    if (1 == argc) {    //错误输入处理
        help();
        return -1;
    }
    FILE* pFile = fopen(argv[1], "rb"); // 数据输入文件
    if (ferror(pFile)) {
        printf("文件错误：不能打开输入文件: %s \n", argv[1]);
        help();
        return -1;
    }
    FILE* outfile;
    if (2 < argc)  // 如果没有输出文件，输出到屏幕
        outfile = fopen(argv[2], "w"); // 输出结果文件
    else
        outfile = stdout;

    bool all_point = false;  // 是否输出所有节点
    if (3 < argc) {
        if (tolower(argv[3][1]) == 'a')
            all_point = true;
    }

    long data_size = get_fileSize(argv[1]);
    int32_t gz_header_3byte = GZ_HEADER_3BYTE;
    fread(&gz_header_3byte, 1, sizeof(gz_header_3byte), pFile);
    rewind(pFile);

    // 判断是否是gz文件
    bool is_gzfile = (GZ_HEADER_3BYTE == (gz_header_3byte & 0xFFFFFF));

    char* buffer = NULL;
    if (is_gzfile) {
        // 读取gz文件到内存
        data_size = get_gzbinSize(argv[1]);

        gzFile gzf = gzopen(argv[1], "rb");
        buffer = new char[data_size + 1];
        buffer[data_size] = 0;

        if (gzread(gzf, buffer,  data_size)  < 0)
            return -1;
        gzclose(gzf);

    } else {
        // 读已经解压开bin文件到内存
        buffer = new char[data_size];
        fread(buffer, 1, data_size, pFile);
    }

    out_google_maps = fopen("index.html", "w");
    fprintf(out_google_maps, "<html><head>\n<meta http-equiv=\"refresh\" content=\"2; url=http://www.google.cn/maps/dir/");

    // 分析内存中的数据
    char* gps_buffer = buffer;
    GPS_FILEHEAD* gps_filehead = (GPS_FILEHEAD*)gps_buffer;   // bin文件头

//    printf("%d\t%d\n", gps_filehead->data_pos, gps_filehead->unknown1);
//    printf("%d\t%d\n", gps_filehead->unknown2, gps_filehead->unknown3);


    // 兼容旧版本 02 04 05 和当前 06版本数据，使用指针回退，兼容数据结构不一样长
    int ver_offset = 0;
    if (gps_filehead->data_ver == 5) {
        ver_offset = sizeof(int32_t);
    } else if (gps_filehead->data_ver <= 4) {
        ver_offset = 2 * sizeof(int32_t);
    }


    GPS_POINT* gps_point = (GPS_POINT*)(gps_buffer + gps_filehead->data_pos);  // 第一条GPS记录
    int gps_point_total = (data_size - gps_filehead->data_pos) / (sizeof(GPS_POINT) - ver_offset); // GPS记录条目数

    fprintf(outfile, "纬度\t经度\t时速(Km/H)\t时间戳\n");

    int google_maps_point = gps_point_total / 10;  // google地图用分10段，分数

    if (all_point) {     // 输出所有GPS节点
        while (gps_point_total--) {
            print_gps_point(outfile, gps_point++);
            gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);    // 兼容旧版本 02 04 05 当作 06版本数据读，读好来个指针回退
        }
    } else {
        int fraction = google_maps_point;   // google地图用分10段，分数
        if (3 < argc) {      // 自定义分数
            int diy_fraction = abs(atoi(argv[3]));
            if (5 < diy_fraction && diy_fraction < 3601)
                fraction = diy_fraction;
            //  printf("%d", diy_fraction);
        }

        int count = fraction;
        while (gps_point_total--) {
            if (count % fraction == 0) {
                print_gps_point(outfile, gps_point);
                count--;
            } else {
                count--;
                if (count == 0)
                    count = fraction;
            }

            gps_point++;
            gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);

        }

        if (3 < argc) {
            gps_point = (GPS_POINT*)((char*)gps_point + ver_offset);
            print_gps_point(outfile, --gps_point); // 终点节点
        }
    }

    // 释放文件和内存
    fclose(pFile);

    fprintf(out_google_maps, "\" />\n</head><body>\n自动跳转到 到google地图 <p>\n http://www.google.cn/maps/dir/ gps坐标/ gps坐标 / gps坐标 <p>\n");
    fprintf(out_google_maps, "当前轨迹数据文件:%s<p>\n</body></html>", argv[1]);
    fclose(out_google_maps);

    delete[] buffer;

    return 0;
}



