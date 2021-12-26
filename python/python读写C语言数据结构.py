from ctypes import *

class Location(Structure):
    _fields_ = [('lon', c_double), ('lat', c_double)]

gps = Location(lon = 118.118, lat = 29.19)

print(gps.lon, gps.lat)

fo = open("gps.dat", "wb")

fo.write(gps)

gps.lon = gps.lon / 2
gps.lat = gps.lat / 2


print(gps.lon, gps.lat)
fo.write(gps)

# gps.dat
# 00000000h: 64 3B DF 4F 8D 87 5D 40 71 3D 0A D7 A3 30 3D 40 ; d;逴崌]@q=.祝0=@
# 00000010h: 64 3B DF 4F 8D 87 4D 40 71 3D 0A D7 A3 30 2D 40 ; d;逴崌M@q=.祝0-@

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

######## 

import struct

fo = open("gps.dat", "rb")

x = (fo.read(8))
y = (fo.read(8))

lon = struct.unpack("d", x)
lat = struct.unpack("d", y)

print((lon[0], lat[0]))

aa = struct.pack("4B", 1, 2, 3, 4)
print(aa)
bb = struct.unpack("f", aa)
print(bb)


