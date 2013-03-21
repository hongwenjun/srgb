//ReadBitMap
//
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


#define   WIDTHBYTES(bits) (((bits)+31)/32*4)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;


//位图文件头信息结构定义
//其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）

typedef struct tagBITMAPFILEHEADER {

    DWORD bfSize;           //文件大小
    WORD   bfReserved1;     //保留字，不考虑
    WORD   bfReserved2;     //保留字，同上
    DWORD bfOffBits;        //实际位图数据的偏移字节数，即前三个部分长度之和
} BITMAPFILEHEADER;


//信息头BITMAPINFOHEADER，也是一个结构，其定义如下：

typedef struct tagBITMAPINFOHEADER {
//public:
    DWORD   biSize;             //指定此结构体的长度，为40
    LONG    biWidth;            //位图宽
    LONG    biHeight;           //位图高
    WORD    biPlanes;           //平面数，为1
    WORD    biBitCount;         //采用颜色位数，可以是1，2，4，8，16，24，新的可以是32
    DWORD   biCompression;      //压缩方式，可以是0，1，2，其中0表示不压缩
    DWORD   biSizeImage;        //实际位图数据占用的字节数
    LONG    biXPelsPerMeter;    //X方向分辨率
    LONG    biYPelsPerMeter;    //Y方向分辨率
    DWORD   biClrUsed;          //使用的颜色数，如果为0，则表示默认值(2^颜色位数)
    DWORD   biClrImportant;     //重要颜色数，如果为0，则表示所有颜色都是重要的
} BITMAPINFOHEADER;




//调色板Palette，当然，这里是对那些需要调色板的位图文件而言的。24位和32位是不需要调色板的。
//（似乎是调色板结构体个数等于使用的颜色数。）

typedef struct tagRGBQUAD {
//public:
    BYTE     rgbBlue; //该颜色的蓝色分量
    BYTE     rgbGreen; //该颜色的绿色分量
    BYTE     rgbRed; //该颜色的红色分量
    BYTE     rgbReserved; //保留值
} RGBQUAD;



void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
    printf("位图文件头:\n");
    printf("文件大小:%d\n", pBmpHead->bfSize);
    printf("保留字:%d\n", pBmpHead->bfReserved1);
    printf("保留字:%d\n", pBmpHead->bfReserved2);
    printf("实际位图数据的偏移字节数:%d\n", pBmpHead->bfOffBits);

}


void showBmpInforHead(tagBITMAPINFOHEADER* pBmpInforHead)
{
    printf("位图信息头:\n");
    printf("结构体的长度:%d\n", pBmpInforHead->biSize);
    printf("位图宽:%d\n", pBmpInforHead->biWidth);
    printf("位图高:%d\n", pBmpInforHead->biHeight);
    printf("biPlanes平面数:%d\n", pBmpInforHead->biPlanes);
    printf("biBitCount采用颜色位数:%d\n", pBmpInforHead->biBitCount);
    printf("压缩方式:%d\n", pBmpInforHead->biCompression);
    printf("biSizeImage实际位图数据占用的字节数:%d\n", pBmpInforHead->biSizeImage);
    printf("X方向分辨率:%d\n", pBmpInforHead->biXPelsPerMeter);
    printf("Y方向分辨率:%d\n", pBmpInforHead->biYPelsPerMeter);
    printf("使用的颜色数:%d\n", pBmpInforHead->biClrUsed);
    printf("重要颜色数:%d\n", pBmpInforHead->biClrImportant);
}

void showRgbQuan(tagRGBQUAD* pRGB)
{
    printf("(%-3d,%-3d,%-3d)   ", pRGB->rgbRed, pRGB->rgbGreen, pRGB->rgbBlue);

}



int main()
{

    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    FILE* pfile;

    char strFile[50];
    printf("please input the .bmp file name:\n");
    scanf("%s", strFile);

    pfile = fopen(strFile, "rb"); //打开文件

    if (pfile != NULL) {
        printf("file bkwood.bmp open success.\n");
        //读取位图文件头信息
        WORD fileType;
        fread(&fileType, 1, sizeof(WORD), pfile);
        if (fileType != 0x4d42) {
            printf("file is not .bmp file!");
            return -1;
        }
        //fseek(pfile,2,SEEK_CUR);   // "BM"
        fread(&bitHead, 1, sizeof(tagBITMAPFILEHEADER), pfile);

        showBmpHead(&bitHead);
        printf("\n\n");

        //读取位图信息头信息
        fread(&bitInfoHead, 1, sizeof(BITMAPINFOHEADER), pfile);
        showBmpInforHead(&bitInfoHead);
        printf("\n");
    } else {
        printf("file open fail!\n");
        return -1;
    }


    tagRGBQUAD* pRgb ;

    if (bitInfoHead.biBitCount < 24) { //有调色板
        //读取调色盘结信息
        long nPlantNum = long(pow(2, double(bitInfoHead.biBitCount)));   //   Mix color Plant Number;
        pRgb = (tagRGBQUAD*)malloc(nPlantNum * sizeof(tagRGBQUAD));
        memset(pRgb, 0, nPlantNum * sizeof(tagRGBQUAD));
        int num = fread(pRgb, 4, nPlantNum, pfile);

        printf("Color Plate Number: %d\n", nPlantNum);

        printf("颜色板信息:\n");
        for (int i = 0; i < nPlantNum; i++) {
            if (i % 5 == 0) {
                printf("\n");
            }
            showRgbQuan(&pRgb[i]);

        }

        printf("\n");

    }


    int width = bitInfoHead.biWidth;
    int height = bitInfoHead.biHeight;
//分配内存空间把源图存入内存
    int l_width   = WIDTHBYTES(width * bitInfoHead.biBitCount); //计算位图的实际宽度并确保它为32的倍数
    BYTE*    pColorData = (BYTE*)malloc(height * l_width);
    memset(pColorData, 0, height * l_width);
    long nData = height * l_width;

//把位图数据信息读到数组里
    fread(pColorData, 1, nData, pfile);



//将位图数据转化为RGB数据
    tagRGBQUAD* dataOfBmp;
    dataOfBmp = (tagRGBQUAD*)malloc(width * height * sizeof(tagRGBQUAD)); //用于保存各像素对应的RGB数据
    memset(dataOfBmp, 0, width * height * sizeof(tagRGBQUAD));

    if (bitInfoHead.biBitCount < 24) { //有调色板，即位图为非真彩色
        int k;
        int index = 0;
        if (bitInfoHead.biBitCount == 1) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    BYTE mixIndex = 0;
                    k = i * l_width + j / 8; //k:取得该像素颜色数据在实际数据数组中的序号
                    //j:提取当前像素的颜色的具体值
                    mixIndex = pColorData[k];
                    switch (j % 8) {
                    case 0:
                        mixIndex = mixIndex << 7;
                        mixIndex = mixIndex >> 7;
                        break;
                    case 1:
                        mixIndex = mixIndex << 6;
                        mixIndex = mixIndex >> 7;
                        break;
                    case 2:
                        mixIndex = mixIndex << 5;
                        mixIndex = mixIndex >> 7;
                        break;

                    case 3:
                        mixIndex = mixIndex << 4;
                        mixIndex = mixIndex >> 7;
                        break;
                    case 4:
                        mixIndex = mixIndex << 3;
                        mixIndex = mixIndex >> 7;
                        break;

                    case 5:
                        mixIndex = mixIndex << 2;
                        mixIndex = mixIndex >> 7;
                        break;
                    case 6:
                        mixIndex = mixIndex << 1;
                        mixIndex = mixIndex >> 7;
                        break;

                    case 7:
                        mixIndex = mixIndex >> 7;
                        break;
                    }

                    //将像素数据保存到数组中对应的位置
                    dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
                    index++;

                }
        }

        if (bitInfoHead.biBitCount == 2) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    BYTE mixIndex = 0;
                    k = i * l_width + j / 4; //k:取得该像素颜色数据在实际数据数组中的序号
                    //j:提取当前像素的颜色的具体值
                    mixIndex = pColorData[k];
                    switch (j % 4) {
                    case 0:
                        mixIndex = mixIndex << 6;
                        mixIndex = mixIndex >> 6;
                        break;
                    case 1:
                        mixIndex = mixIndex << 4;
                        mixIndex = mixIndex >> 6;
                        break;
                    case 2:
                        mixIndex = mixIndex << 2;
                        mixIndex = mixIndex >> 6;
                        break;
                    case 3:
                        mixIndex = mixIndex >> 6;
                        break;
                    }

                    //将像素数据保存到数组中对应的位置
                    dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
                    index++;


                }
        }
        if (bitInfoHead.biBitCount == 4) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    BYTE mixIndex = 0;
                    k = i * l_width + j / 2;
                    mixIndex = pColorData[k];
                    if (j % 2 == 0) {
                        //低
                        mixIndex = mixIndex << 4;
                        mixIndex = mixIndex >> 4;
                    } else {
                        //高
                        mixIndex = mixIndex >> 4;
                    }

                    dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
                    index++;

                }

        }
        if (bitInfoHead.biBitCount == 8) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    BYTE mixIndex = 0;

                    k = i * l_width + j;

                    mixIndex = pColorData[k];

                    dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
                    index++;



                }
        }
        if (bitInfoHead.biBitCount == 16) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    WORD mixIndex = 0;

                    k = i * l_width + j * 2;
                    WORD shortTemp;
                    shortTemp = pColorData[k + 1];
                    shortTemp = shortTemp << 8;

                    mixIndex = pColorData[k] + shortTemp;

                    dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
                    index++;
                }
        }
    } else { //位图为24位真彩色
        int k;
        int index = 0;
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                k = i * l_width + j * 3;
                dataOfBmp[index].rgbRed = pColorData[k + 2];
                dataOfBmp[index].rgbGreen = pColorData[k + 1];
                dataOfBmp[index].rgbBlue = pColorData[k];
                index++;
            }
    }


    printf("像素数据信息:\n");
    for (int i = 0; i < width * height; i++) {
        if (i % 5 == 0) {
            printf("\n");
        }
        showRgbQuan(&dataOfBmp[i]);
    }

    fclose(pfile);
    if (bitInfoHead.biBitCount < 24) {
        free(pRgb);
    }
    free(dataOfBmp);
    free(pColorData);
    printf("\n");

}
