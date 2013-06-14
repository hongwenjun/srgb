#include <iostream>
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


int main(int argc , char* argv[])
{
    using namespace std;

    FILE* cdr_pfile = fopen(argv[1] , "rb");
    if (cdr_pfile == NULL)
        return 88;



    // 读RIFF_CDR_TYPE 文件头和CDR_VERSION 软件版本号
    RIFF_CDR_TYPE cdr_riff;
    CDR_VERSION cdr_vrsn;
    WORD version;
    fread(&cdr_riff , 1 , sizeof(cdr_riff) , cdr_pfile);
    fread(&cdr_vrsn , 1 , sizeof(cdr_vrsn) , cdr_pfile);
    fread(&version , 1 , sizeof(version) , cdr_pfile);

    size_t pos = ftell(cdr_pfile);

    if (RIFF_HEADER == cdr_riff.riff && CDr9_TYPE == cdr_riff.file_type) {
        printf("%s %d\n", "CorelDRAW 简体中文版格式" , version / 100);
    }

    if (RIFF_HEADER == cdr_riff.riff && (FCC('CDR*') << 8) == (cdr_riff.file_type << 8)) {
        printf("%s %d\n", "CorelDRAW 文件格式" , version / 100);
    }

    RIFFCHUNK disp_chunk;
    fread(&disp_chunk , 1 , sizeof(disp_chunk) , cdr_pfile);

    while (FCC('DISP') != disp_chunk.fcc && !feof(cdr_pfile)) {

        fseek(cdr_pfile, disp_chunk.cb, SEEK_CUR);
        fread(&disp_chunk , 1 , sizeof(disp_chunk) , cdr_pfile);
    }


    size_t raw_size = disp_chunk.cb;
    if (FCC('DISP') == disp_chunk.fcc) {
        printf("对齐--> 对象大小%d\n", disp_chunk.cb);

        char* disp_buf = new char[raw_size + 10];

        fread(disp_buf + 10, 1, raw_size , cdr_pfile);
        for (int i = 0 ; i != 10 ; i++)
            disp_buf[i] = '\0';

        disp_buf[0] = 'B';
        disp_buf[1] = 'M';
        disp_buf[10] = 0x36;   // 图像工具建立的BMP文件，256色索引
        disp_buf[11] = 0x04;   // 这里好像都是 0x3604 ，强制填，，临时解决了


        size_t* bmpsize = (size_t*)(disp_buf + 2);
        *bmpsize = raw_size + 10;


        FILE* bmp_dispfile;
        bmp_dispfile = fopen("DISP_BMP.bmp" , "wb");
        fwrite(disp_buf, 1, raw_size + 10, bmp_dispfile);

        fclose(bmp_dispfile);

        cout << "DISP_BMP.bmp 文件头:" << disp_buf << endl;

        delete[] cdr_pfile;

    }
    fclose(cdr_pfile);
    return 0;
}



#if(0)
00000000h: 52 49 46 46 C4 AE 00 00 43 44 72 39 76 72 73 6E ; RIFF漠..CDr9vrsn
00000010h: 02 00 00 00 84 03 44 49 53 50 2C 28 00 00 08 00 ; ...._DISP__....

#endif


