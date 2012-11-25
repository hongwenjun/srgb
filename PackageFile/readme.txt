PkFile 1.00  Copyright (c) Hongwenjun(蘭公子)  2012-11-28

Usage: PkFile.exe  -p{打包路径}   -o{输出文件}   -e{正则公式}

示 例: PkFile.exe  -pD:\我的照片  -oD:\Temp\backup.tar  -e2012-11
       可以打包指定目录2012年11月建立或修改的文件

直接修改配置文件 PACKAGE_FILE.ini 中的PATH和REGEX 参数，等价控制台输入参数

/***************** 就当使用说明吧 ^_^ *****************/

为什么写这个工具？这个工具有什么用？

	我有一个工作目录，2004年存档以来已经有50GB了，我保存文件算是比较好了，
把文件做的尽量小了再保存。听一个同行说，他的工作数据现在已经有1TB多了。
我本来是使用同步软件双机备份，几个月同步一次，家里笔记本半年多和工作电脑
同步一次。想找个差异备份软件，好像找不到好。
	所以就自己写了这个正则打包工具，为自己服务，和可能用的到人服务。

具个示例操作：我想把 2012年10月到12月数据打包成一个包，用U盘异地备份
连续输入以下三次命令
PkFile.exe  -pD:\工作目录  -oD:\Temp\backup.tar  -e2012-10
PkFile.exe  -pD:\工作目录  -oD:\Temp\backup.tar  -e2012-11
PkFile.exe  -pD:\工作目录  -oD:\Temp\backup.tar  -e2012-12

就会把 2012年10月到12月三个月的文件打包到 backup.tar
因为数据一般都是已经压缩过，用tar只打包，速度上会快很多

对于不习惯控制台操作的，只要 
PACKAGE_FILE.ini 只留三行
PATH  =   D:\工作目录 
REGEX =   2012-10
PACKFILE = D:\Temp\backup.tar
然后双击 PkFile.exe 就可以备份了



/*****************   PACKAGE_FILE.ini  文件示例   *************************
;直接修改配置文件 = PACKAGE_FILE.ini 中的PATH和REGEX 参数，等价控制台输入参数
[PACKAGE_FILE]
PATH  =   D:\工作目录 
REGEX =   2012-10
PACKFILE = D:\Temp\backup.tar
***************************************************************************/

