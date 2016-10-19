#include <stdio.h>

#define BSWAP_8(x)  ((x) & 0xff)
#define BSWAP_16(x) ((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x) ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x) ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))

int main()
{
    int i = 0x12345678;
    FILE* pFile;
    pFile = fopen("myfile.bin", "wb");
    fwrite(&i, sizeof(i), 1, pFile);

//    i = BSWAP_32(i);  // i反序

// VC编译器 使用 bswap 汇编命令
    __asm{
        mov eax, [i];
        bswap eax;
        mov [i], eax;
    }

    printf("%x", i);
    fwrite(&i, sizeof(i), 1, pFile);
    fclose(pFile);
}





