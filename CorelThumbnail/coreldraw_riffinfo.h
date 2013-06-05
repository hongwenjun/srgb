// coreldraw_riffinfo.h

#ifndef CORELDRAW_RIFFINFO_H_INCLUDED
#define CORELDRAW_RIFFINFO_H_INCLUDED

#include <windows.h>
#include <stdio.h>

//  'ABCD' == FCC('DCBA')    数字(ch4)反序
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))

#define RIFF_HEADER FCC('RIFF')
#define CDr9_TYPE   FCC('CDr9')    // RIFF....CDr9vrsn   CorelDRAW 9 简体中文版

#define CDR8_TYPE   FCC('CDR8')
#define CDR9_TYPE   FCC('CDR9')
#define CDRA_TYPE   FCC('CDRA')    // CorelDRAW 10  文件类型
#define CDRB_TYPE   FCC('CDRB')
#define CDRC_TYPE   FCC('CDRC')
#define CDRD_TYPE   FCC('CDRD')    // CorelDRAW X3 (13) 文件类型

#define CDRE_TYPE   FCC('CDRE')    // CorelDRAW X4  文件类型 , 开始使用 PK文件头 zip打包
#define CDRF_TYPE   FCC('CDRF')    // CorelDRAW X5

typedef struct coreldraw_riff_head {
    FOURCC  riff;       // 必须为 'RIFF'
    DWORD   cb;     // CorelDRAW 数据文件大小
    DWORD   file_type;  // 一般为 'CDR?'
} RIFF_CDR_TYPE;

typedef struct _riffchunk {
    FOURCC fcc;
    DWORD  cb;
} RIFFCHUNK, * LPRIFFCHUNK , CDR_VERSION;

// 获得 CorelDRAW X3文件 为止的版本号
int get_cdrfile_version(const char* cdr_filename);

// 获得 CorelDRAW X3文件 为止 cdr_filename  展开缩略图 bmp_filename
bool cdr_riff_disp2bmp(const char* cdr_filename, const char* bmp_filename);



#endif // CORELDRAW_RIFFINFO_H_INCLUDED
