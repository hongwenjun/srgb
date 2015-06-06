
```

-fexec-charset=gbk   // 可执行文件中的中文字符资源为 GBK，控制台显示中文
-finput-charset=UTF-8 // 源代码 main.cpp 为 UTF-8编码

#include <iostream>

using namespace std;

int main()
{
    cout << "你好世界!" << endl;
    return 0;
}

// 输出
你好世界!
```


[50.0%](.md) g++.exe  -fexec-charset=gbk -finput-charset=UTF-8    -c D:\1myapp\CPP4\11\main.cpp -o obj\Release\main.o
[100.0%] g++.exe  -o bin\Release\11.exe obj\Release\main.o   -s


gcc 编译器 编译的配置文件要有 --enable-libiconv 参数才能进行 以上转码


可以执行 gcc -v 看你的gcc 支持哪些配置
```
$ gcc -v

使用内建 specs。
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=c:/codeblocks/build/gcc/bin/../libexec/gcc/i686-pc-mingw32/4
.5.3/lto-wrapper.exe
目标：i686-pc-mingw32
配置为：./configure --prefix=/c/gcc --host=i686-pc-mingw32 --build=i686-pc-mingw
32 --target=i686-pc-mingw32 --with-lto-plugin --with-host-libstdcxx='-lstdc++ -l
supc++ -lm' --disable-bootstrap --disable-werror --enable-languages=c,c++,fortra
n --enable-libgomp --enable-threads=win32 --enable-lto --with-system-zlib --enab
le-libstdcxx-debug --enable-version-specific-runtime-libs --enable-fully-dynamic
-string --disable-sjlj-exceptions --with-dwarf2 --enable-libiconv --disable-symv
ers --enable-plugins --enable-cloog-backend=ppl --enable-static --disable-shared
 --enable-nls --without-included-gettext --disable-win32-registry --with-pkgvers
ion=pcx32
线程模型：win32
gcc 版本 4.5.3 20110310 (prerelease) (pcx32)
```