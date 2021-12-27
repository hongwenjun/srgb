from ctypes import *
class Location(Structure):
    _fields_ = [('lon', c_double), ('lat', c_double)]

gps = Location(lon=118.118, lat=29.19)
print(gps.lon, gps.lat)

f = open("gps.dat", "wb")
f.write(gps)

gps.lon = gps.lon / 10
gps.lat = gps.lat + 40.0005

print(gps.lon, gps.lat)
f.write(gps) ; f.flush(); f.close()
# gps.dat
# 00000000h: 64 3B DF 4F 8D 87 5D 40 71 3D 0A D7 A3 30 3D 40 ;
# 00000010h: 1D C9 E5 3F A4 9F 27 40 D5 78 E9 26 31 4C 51 40 ;

'''
#include <stdio.h>
struct gps {
    double x;
    double y;
};

int main()
{
    struct gps g;
    FILE*  f = fopen("gps.dat", "rb");

    fread(&g, 1, sizeof(g), f);
    printf("%f  %f", g.x, g.y);

    return 0;
}
'''

import struct
from ctypes import *
class Location(Structure):
    _fields_ = [('lon', c_double), ('lat', c_double)]

gps = Location()
f = open("gps.dat", "rb")

for i in range(2):
    tmp = f.read(16)
    print(tmp)
    tmp = struct.unpack("2d", tmp)

    gps.lon = tmp[0]
    gps.lat = tmp[1]

    print(gps.lon, gps.lat)

# struct --- 将字节串解读为打包的二进制数据
# 此模块可以执行 Python 值和以 Python bytes 对象表示的 C 结构之间的转换。 
# 这可以被用来处理存储在文件中或是从网络连接等其他来源获取的二进制数据。
# 它使用 格式字符串 作为 C 结构布局的精简描述以及与 Python 值的双向转换。
aa = struct.pack("4B", 1, 2, 3, 4)
print(aa)
bb = struct.unpack("f", aa)
print(bb)

## Python读写C语言数据结构 总结精简
import struct
from ctypes import *
class Location(Structure):
    _fields_ = [('lon', c_double), ('lat', c_double)]

f = open("gps.dat", "rb")
t = f.read(16) ; print(t)
t = struct.unpack("2d", t) ; print(t)
gps =Location(lon=t[0], lat=t[1])
print(gps.lon, gps.lat)
