//定义每个Byte中有8个Bit位
#include <memory.h>
#include <stdio.h>

#define BYTESIZE 8
void SetBit(char* p, int posi)
{
    for (int i = 0; i < (posi / BYTESIZE); i++) {
        p++;
    }

    *p = *p | (0x01 << (posi % BYTESIZE)); //将该Bit位赋值1
    return;
}

void BitMapSortDemo()
{
    //为了简单起见,我们不考虑负数
    int num[] = {3, 5, 2, 10, 6, 12, 8, 14, 9};
    //BufferLen这个值是根据待排序的数据中最大值确定的
    //待排序中的最大值是14,因此只需要2个Bytes(16个Bit)
    //就可以了.
    const int BufferLen = 2;
    char* pBuffer = new char[BufferLen];
    //要将所有的Bit位置为0,否则结果不可预知.
    memset(pBuffer, 0, BufferLen);
    for (int i = 0; i < 9; i++) {
        //首先将相应Bit位上置为1
        SetBit(pBuffer, num[i]);
    }

    //输出排序结果
    for (int i = 0; i < BufferLen; i++) { //每次处理一个字节(Byte)
        for (int j = 0; j < BYTESIZE; j++) { //处理该字节中的每个Bit位
            //判断该位上是否是1,进行输出,这里的判断比较笨.
            //首先得到该第j位的掩码(0x01<<j),将内存区中的
            //位和此掩码作与操作.最后判断掩码是否和处理后的
            //结果相同
            if ((*pBuffer & (0x01 << j)) == (0x01 << j)) {
                printf("%d ", i * BYTESIZE + j);
            }
        }
        pBuffer++;
    }
}

int main(int argc, char *argv[])
{
    BitMapSortDemo();
    return 0;
}


/*     http://blog.csdn.net/v_july_v/article/details/6685962
海量数据处理之Bti-map详解
    Bloom Filter已在上一篇文章海量数据处理之Bloom Filter详解中予以详细阐述，本文接下来着重阐述Bit-map。有任何问题，欢迎不吝指正。

什么是Bit-map
    所谓的Bit-map就是用一个bit位来标记某个元素对应的Value， 而Key即是该元素。由于采用了Bit为单位来存储数据，因此在存储空间方面，可以大大节省。

    如果说了这么多还没明白什么是Bit-map，那么我们来看一个具体的例子，假设我们要对0-7内的5个元素(4,7,2,5,3)排序（这里假设这些元素没有重复）。那么我们就可以采用Bit-map的方法来达到排序的目的。要表示8个数，我们就只需要8个Bit（1Bytes），首先我们开辟1Byte的空间，将这些空间的所有Bit位都置为0(如下图：)



    然后遍历这5个元素，首先第一个元素是4，那么就把4对应的位置为1（可以这样操作 p+(i/8)|(0×01<<(i%8)) 当然了这里的操作涉及到Big-ending和Little-ending的情况，这里默认为Big-ending）,因为是从零开始的，所以要把第五位置为一（如下图）：



然后再处理第二个元素7，将第八位置为1,，接着再处理第三个元素，一直到最后处理完所有的元素，将相应的位置为1，这时候的内存的Bit位的状态如下：



然后我们现在遍历一遍Bit区域，将该位是一的位的编号输出（2，3，4，5，7），这样就达到了排序的目的。下面的代码给出了一个BitMap的用法：排序。
*/
