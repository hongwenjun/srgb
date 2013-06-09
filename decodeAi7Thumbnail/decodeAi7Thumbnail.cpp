#include <iostream>
#include <string>
#include <windows.h>>
#include <stdio.h>

using namespace std;

// 从别人代码里移植，还没调试正确
string decodeHex(const byte* src, long srcSize)
{


//    00000030h: 00 01 02 03 04 05 06 07 08 09 10 10 10 10 10 10 ;
//    00000040h: 10 0D 0A 0B 0C 0D 0E 0F 10 10 10 10 10 10 10 10 ;
//    00000050h: 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 ;
//    00000060h: 10 10 0D 0A 0B 0C 0D 0E 0F 10 10 10 10 10 10 10 ;
    // create decoding table
    byte invalid = 16;
    byte decodeHexTable[256];
    for (long i = 0; i < 256; i++) decodeHexTable[i] = invalid;
    for (byte i = 0; i < 10; i++) decodeHexTable[static_cast<byte>('0') + i] = i;
    for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('A') + i] = i + 10;
    for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('a') + i] = i + 10;


    // calculate dest size
    long validSrcSize = 0;
    for (long srcPos = 0; srcPos < srcSize; srcPos++) {
        if (decodeHexTable[src[srcPos]] != invalid) validSrcSize++;
    }
    const long destSize = validSrcSize / 2;

    // allocate dest buffer
    string dest('a', destSize);

    // decode
    for (long srcPos = 0, destPos = 0; destPos < destSize; destPos++) {
        byte buffer = 0;
        for (int bufferPos = 1; bufferPos >= 0 && srcPos < srcSize; srcPos++) {
            byte srcValue = decodeHexTable[src[srcPos]];
            if (srcValue == invalid) continue;
            buffer |= srcValue << (bufferPos * 4);
            bufferPos--;
        }
        dest[destPos] = buffer;
    }

    return dest;
}


int main()
{
    const char* hexs =
        "%0066000066330066660066990066CC0066FF009900009933009966009999"
        "%0099CC0099FF00CC0000CC3300CC6600CC9900CCCC00CCFF00FF3300FF66"
        "%00FF9900FFCC3300003300333300663300993300CC3300FF333300333333";

    string str = decodeHex((BYTE*)hexs , 240);
    cout << hexs;
    FILE* pFile;
    pFile = fopen("myfile.bin" , "wb");
    fwrite(str.c_str() , 1 , str.size(), pFile);
    return 0;
}


#if(0)
/*!
  @brief Decode a Hex string.
 */
DataBuf decodeHex(const byte* src, long srcSize);


/*!
  @brief Decode an Illustrator thumbnail that follows after %AI7_Thumbnail.
 */
DataBuf decodeAi7Thumbnail(const DataBuf& src);

== == == == == == == == == == == == == == == == == == == =

    DataBuf decodeHex(const byte* src, long srcSize)
{
    // create decoding table
    byte invalid = 16;
    byte decodeHexTable[256];
    for (long i = 0; i < 256; i++) decodeHexTable[i] = invalid;
    for (byte i = 0; i < 10; i++) decodeHexTable[static_cast<byte>('0') + i] = i;
    for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('A') + i] = i + 10;
    for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('a') + i] = i + 10;

    // calculate dest size
    long validSrcSize = 0;
    for (long srcPos = 0; srcPos < srcSize; srcPos++) {
        if (decodeHexTable[src[srcPos]] != invalid) validSrcSize++;
    }
    const long destSize = validSrcSize / 2;

    // allocate dest buffer
    DataBuf dest(destSize);

    // decode
    for (long srcPos = 0, destPos = 0; destPos < destSize; destPos++) {
        byte buffer = 0;
        for (int bufferPos = 1; bufferPos >= 0 && srcPos < srcSize; srcPos++) {
            byte srcValue = decodeHexTable[src[srcPos]];
            if (srcValue == invalid) continue;
            buffer |= srcValue << (bufferPos * 4);
            bufferPos--;
        }
        dest.pData_[destPos] = buffer;
    }
    return dest;
}


DataBuf decodeAi7Thumbnail(const DataBuf& src)
{
    const byte* colorTable = src.pData_;
    const long colorTableSize = 256 * 3;
    if (src.size_ < colorTableSize) {
#ifndef SUPPRESS_WARNINGS
        EXV_WARNING << "Invalid size of AI7 thumbnail: " << src.size_ << "\n";
#endif
        return DataBuf();
    }
    const byte* imageData = src.pData_ + colorTableSize;
    const long imageDataSize = src.size_ - colorTableSize;
    const bool rle = (imageDataSize >= 3 && imageData[0] == 'R' && imageData[1] == 'L' && imageData[2] == 'E');
    std::string dest;
    for (long i = rle ? 3 : 0; i < imageDataSize;) {
        byte num = 1;
        byte value = imageData[i++];
        if (rle && value == 0xFD) {
            if (i >= imageDataSize) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Unexpected end of image data at AI7 thumbnail.\n";
#endif
                return DataBuf();
            }
            value = imageData[i++];
            if (value != 0xFD) {
                if (i >= imageDataSize) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Unexpected end of image data at AI7 thumbnail.\n";
#endif
                    return DataBuf();
                }
                num = value;
                value = imageData[i++];
            }
        }
        for (; num != 0; num--) {
            dest.append(reinterpret_cast<const char*>(colorTable + (3 * value)), 3);
        }
    }
    return DataBuf(reinterpret_cast<const byte*>(dest.data()), static_cast<long>(dest.size()));
}



#endif
