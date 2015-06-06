# TDMGCC 编译 Boost库 #

## 1. 开始  运行 输入下面，开启一个控制台 ##
cmd /k  D:\CodeBlocks\MinGW32\mingwvars.bat

## 2. 先编译  bjam.exe 和 b2.exe ##
cd D:\boost\_1\_51\_0\tools\build\v2\engine
然后输入 build.bat mingw

## 3. 列出的是所有需要编译的boost模块 ##
cd D:\boost\_1\_51\_0
bjam --show-libraries
The following libraries require building:
> - chrono
> - context
> - date\_time
> - exception
> - filesystem
> - graph
> - graph\_parallel
> - iostreams
> - locale
> - math
> - mpi
> - program\_options
> - python
> - random
> - regex
> - serialization
> - signals
> - system
> - test
> - thread
> - timer
> - wave
## 4. 编译 release 和Debug 库 ##
```
// 本人还没编译，待测试修改参数
bjam install --toolset=gcc --prefix="D:\boost" release --without-wave --without-mpi --without-python --without-math --without-graph

bjam install --toolset=gcc --prefix="D:\boost" debug --without-wave --without-mpi --without-python --without-math --without-graph
```

## 5. VS系统可以下载别人编译好的Boost库 ##
http://www.boostpro.com/download/

BoostPro 1.51.0 Installer (64-bit) (195K .exe)
BoostPro 1.51.0 Installer (32-bit) (200K .exe)