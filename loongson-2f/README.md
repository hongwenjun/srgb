# 龙芯2F 8089D 安装 debian 8.10 使用恢复系统和升级法
- 本文章短网址  https://git.io/loongson
### 1. 搜索 loongson2_debian8_xfce4_20141129a.tar.lzma  
-	http://www.anheng.com.cn/loongson/install/      安恒源
- https://mirrors.tuna.tsinghua.edu.cn/loongson/  清华源
- 下载 　 loongson2_debian8_xfce4_20141129a.tar.lzma  　 vmlinux  　 readme.utf8.txt
- 在企鹅云也会搜到，看哪个地方块，哪里下载到本地

### 2. 龙芯2F_8089D网络安装，官方源已经失效，恢复法也可以这个方法
- 本项目 已经有 tftp_sever.exe，可以使用这个简单点
```
Press DEL (repeatedly) on startup to enter PMON (the Yeeloong's BIOS), and then:

PMON> ifaddr rtl0 192.168.1.135                # 这行不用也没关系 
PMON> load tftp://192.168.1.188/vmlinux        # 看你情况用哪个核心
PMON> initrd tftp://192.168.1.188/initrd.gz    # 恢复法，不用这行
PMON> g
```

# debian 8.11 不支持 mipsle 的龙芯2F了,自己搞了一天两夜(N次网络和覆盖安装才顿悟)

### 3. 使用 Debian mipsel DVD 光盘做软件源
- 搜索 debian-8.10.0-mipsel-DVD-1.iso 能否找到看人品
- 推荐使用国外的VPS，下载镜像，挂载在web目录
- wget https://cdimage.debian.org/cdimage/archive/8.10.0/mipsel/iso-dvd/debian-8.10.0-mipsel-DVD-1.iso

```
# 临时做软件源 下载ISO 加载  mount -o loop debian-8.10.0-mipsel-DVD-1.iso debian
# 编辑 vim /etc/apt/sources.list
deb http://srgb18.ga/debian/debian/ jessie main contrib
deb http://srgb18.ga/debian/ jessie main contrib
```

### 4.  卸载GUI xfec4 窗口和安装设置软件
```
apt remove xfce4  x11-common 
apt update
apt install tmux   # 后台窗口
apt upgrade        # tmux 后更新软件
apt install gcc g++ gdb git build-essential autoconf libtool automake  htop  screen nginx qemu 
```

### 5. 使用 QEMU 可以用户态模拟翻译运行 GO语言 1.10版本以下编译的程序
```
qemu-mips    ./frpc --help 
qemu-mipsle  ./frpc_mipsle
```
- [frp  mips](https://github.com/hongwenjun/srgb/tree/master/loongson-2f/frp)    [kcp-client mips](https://github.com/hongwenjun/srgb/raw/master/loongson-2f/bin/kcp-client)

----
### MIPS汇编语言小科普 - 视频教程
```
链接: https://pan.baidu.com/s/1gRNMUpoZtZulmHv7PwkLtQ 密码: ygce

Mars4_5.jar   MIPS 模拟器  需要安装 自行安装 JAVA

mips_mars_help.zip  指令集使用 和 包含
mipsref.pdf   MIPS/SPIM Reference Card
```
----
```
其他相关链接

龙芯汇编语言程序设计
https://www.bilibili.com/video/av14809674/

mips指令简单入门
https://blog.csdn.net/ben_chong/article/details/51794392

MIPS体系结构--指令集
https://blog.csdn.net/sdreamq/article/details/51044913
```
