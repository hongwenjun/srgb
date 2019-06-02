# 龙芯2F 8089D 安装 debian 8.10 使用恢复系统和升级法

### 1. 搜索 loongson2_debian8_xfce4_20141129a.tar.lzma  
-	http://www.anheng.com.cn/loongson/install/
-	下载  loongson2_debian8_xfce4_20141129a.tar.lzma   vmlinux   readme.utf8.txt
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
