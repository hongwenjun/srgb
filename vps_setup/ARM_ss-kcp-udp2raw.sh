
# 纯净编译脚本 shadowsocks-libev For Debian / Ubuntu / Centos
wget -qO- https://git.io/ss.ss | bash 


# 适合 ARM64 的 udp2raw和Kcptun安装
udp2raw_update()
{
        systemctl stop rc-local

    # 下载 UDP2RAW
    udp2raw_ver=$(wget --no-check-certificate -qO- https://api.github.com/repos/wangyu-/udp2raw-tunnel/releases/latest | grep 'tag_name' | cut -d\" -f4)
    wget https://github.com/wangyu-/udp2raw-tunnel/releases/download/${udp2raw_ver}/udp2raw_binaries.tar.gz
    tar xf udp2raw_binaries.tar.gz
    mv udp2raw_arm /usr/bin/udp2raw
    rm udp2raw* -rf
    rm version.txt

    # 下载 KCPTUN
    kcp_ver=$(wget --no-check-certificate -qO- https://api.github.com/repos/xtaci/kcptun/releases/latest | grep 'tag_name' | cut -d\" -f4)
    kcp_gz_ver=${kcp_ver:1:8}

    kcptun_tar_gz=kcptun-linux-arm64-${kcp_gz_ver}.tar.gz
    wget https://github.com/xtaci/kcptun/releases/download/${kcp_ver}/$kcptun_tar_gz
    tar xf $kcptun_tar_gz
    mv server_linux_arm64 /usr/bin/kcp-server
    rm $kcptun_tar_gz
    rm client_linux_arm64

    # 下载 UDPspeeder
    udpspeeder_ver=$(wget --no-check-certificate -qO- https://api.github.com/repos/wangyu-/UDPspeeder/releases/latest | grep 'tag_name' | cut -d\" -f4)
    wget https://github.com/wangyu-/UDPspeeder/releases/download/${udpspeeder_ver}/speederv2_binaries.tar.gz
    tar xf speederv2_binaries.tar.gz
    mv speederv2_arm /usr/bin/speederv2
    rm speederv2* -rf
    rm version.txt

    systemctl restart rc-local
    ps aux | grep -e kcp -e udp -e speed -e ss-server
    ss-server -h | head -2  && kcp-server -v && udp2raw -h | head -2 && speederv2 -h | head -2

}

udp2raw_update