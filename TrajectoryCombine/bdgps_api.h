#ifndef BDGPS_API_H_INCLUDED
#define BDGPS_API_H_INCLUDED
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <zlib.h>


#define GZ_HEADER_3BYTE 0x088B1F


typedef struct baidu_navi_gps_trajectory_point {
    int32_t longitude;  // 经度   5位小数
    int32_t latitude;   // 纬度   5位小数
    int32_t speed;      // 速度(m/s)   2位小数
    int32_t bearing;    // 方位角  2位小数
    int32_t accuracy;   // 定位精度  2位小数
    int32_t timestamp;  // 时间戳  time_t (32bit)

    int32_t unknown_1;  // 未知字段 9个数字
    int32_t unknown_2;
    int32_t unknown_3;
    int32_t unknown_4;
    int32_t unknown_5;
    int32_t unknown_6;
    int32_t unknown_7;
    int32_t unknown_8;
    int32_t unknown_9;

    int32_t unknown_ver05;  // 05版本的轨迹文件增加了个数据，现在每节点64字节了
    int32_t unknown_ver06;  // 06版本的轨迹文件增加了个数据，现在每节点68字节了

} GPS_POINT;

/*
longitude   latitude     speed    bearing    accuracy   timestamp           unknown ....................... unknown
11946524    2920778       375     28820        500     1419641095           0   0   0   0   0   0   0   0   8179

经度        纬度       速度(m/s)   方位角    定位精度        时间戳           未知 ... ... ... ... .... 未知
119.46524   29.20778    3.75       288.20       5.0    2014.12.27 12:22:17
*/
typedef struct baidu_navi_gps_trajectory_filehead {

    int32_t empty_1;
    int32_t date_pos;  // gps点数据开始
    int32_t empty_2;
    int32_t unknown1;
    int32_t unknown2;
    int32_t unknown3;


} GPS_FILEHEAD;



void help(); // 调用使用帮助

// 打印gps时间戳到文件
void print_gps_point(FILE* outfile, GPS_POINT* gps_point);

// 获取gz文件的数据大小
size_t get_gzbinSize(const char* filename);

// 检查一个文件是否存在
BOOL IsFileExist(LPCTSTR lpFileName);

// 获得文件大小
size_t get_fileSize(const char* filename);

// 功能 获得当前路径
char* GetAppDir(char* szPath);

// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath);


#endif // BDGPS_API_H_INCLUDED
