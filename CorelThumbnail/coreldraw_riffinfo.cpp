#include "coreldraw_riffinfo.h"



// 00000000h: 52 49 46 46 C4 AE 00 00 43 44 72 39 76 72 73 6E ; RIFF漠..CDr9vrsn
// 00000010h: 02 00 00 00 84 03 44 49 53 50 2C 28 00 00 08 00 ; ...._DISP__....

// 读RIFF_CDR_TYPE 文件头和CDR_VERSION 软件版本号
static RIFF_CDR_TYPE cdr_riff;
static CDR_VERSION cdr_vrsn;
static WORD cdr_version;

// 获得 CorelDRAW X3文件 为止的版本号
int get_cdrfile_version(const char* cdr_filename)
{
    FILE* cdr_pfile = fopen(cdr_filename , "rb");

    if (cdr_pfile == NULL)
        return -1;     // 文件不能读 返回 -1

    fread(&cdr_riff , 1 , sizeof(cdr_riff) , cdr_pfile);

    if ('PK' == FCC(cdr_riff.riff << 16))
        return 1400;     // 不能识别版本和高于 X3版本 返回 1400

    fread(&cdr_vrsn , 1 , sizeof(cdr_vrsn) , cdr_pfile);
    fread(&cdr_version , 1 , sizeof(cdr_version) , cdr_pfile);



    fclose(cdr_pfile);
    return cdr_version;
}

// CorelDRAW 13版本之前文件 cdr_filename  展开缩略图 bmp_filename
bool cdr_riff_disp2bmp(const char* cdr_filename, const char* bmp_filename)
{

    FILE* cdr_pfile = fopen(cdr_filename , "rb");

    rewind(cdr_pfile);
    fread(&cdr_riff , 1 , sizeof(cdr_riff) , cdr_pfile);
    fread(&cdr_vrsn , 1 , sizeof(cdr_vrsn) , cdr_pfile);
    fread(&cdr_version , 1 , sizeof(cdr_version) , cdr_pfile);


    // 检索 DISP_chunk 的数据是bmp缩略图数据
    RIFFCHUNK disp_chunk;
    fread(&disp_chunk , 1 , sizeof(disp_chunk) , cdr_pfile);
    // 跳过非 DISP 数据区
    while (FCC('DISP') != disp_chunk.fcc && !feof(cdr_pfile)) {

        fseek(cdr_pfile, disp_chunk.cb, SEEK_CUR);
        fread(&disp_chunk , 1 , sizeof(disp_chunk) , cdr_pfile);
    }


    // DISP_chunk 的数据是bmp缩略图数据，前面加10字节放 用来放'BM'文件头
    size_t raw_size = disp_chunk.cb;
    if (FCC('DISP') == disp_chunk.fcc) {

        char* disp_buf = new char[raw_size + 10];
        fread(disp_buf + 10, 1, raw_size , cdr_pfile);
        for (int i = 0 ; i != 10 ; i++)
            disp_buf[i] = '\0';

        disp_buf[0] = 'B';
        disp_buf[1] = 'M';

        size_t* bmpsize = (size_t*)(disp_buf + 2);
        *bmpsize = raw_size + 10;

        FILE* bmp_dispfile;
        bmp_dispfile = fopen(bmp_filename , "wb");
        fwrite(disp_buf, 1, raw_size + 10, bmp_dispfile);

        fclose(bmp_dispfile);
        delete[] cdr_pfile;
    } else
        return false;

    fclose(cdr_pfile);

    return true;
}
