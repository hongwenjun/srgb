#!/usr/bin/env bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

# tcp 4个  udp 4个  初始化安全防火墙规则预设端口
tcp_port=(80 443 8000 9000)
udp_port=(2999 3999 8000 9999)

# 保存防火墙规则文件路径 /etc/iptables/rules.v4  /etc/iptables/rules.v6
mkdir -p /etc/iptables

# 定义文字颜色
Green="\033[32m"  && Red="\033[31m" && GreenBG="\033[42;37m" && RedBG="\033[41;37m" && Font="\033[0m"

# 检查系统
check_sys(){
    if [[ -f /etc/redhat-release ]]; then
        release="centos"
    elif cat /etc/issue | grep -q -E -i "debian"; then
        release="debian"
    elif cat /etc/issue | grep -q -E -i "ubuntu"; then
        release="ubuntu"
    elif cat /etc/issue | grep -q -E -i "centos|red hat|redhat"; then
        release="centos"
    elif cat /proc/version | grep -q -E -i "debian"; then
        release="debian"
    elif cat /proc/version | grep -q -E -i "ubuntu"; then
        release="ubuntu"
    elif cat /proc/version | grep -q -E -i "centos|red hat|redhat"; then
        release="centos"
    fi
    bit=`uname -m`
}

# 添加指定tcp端口防火墙规则
Add_iptables(){
    iptables -I INPUT -m state --state NEW -m tcp -p tcp --dport ${port} -j ACCEPT
    ip6tables -I INPUT -m state --state NEW -m tcp -p tcp --dport ${port} -j ACCEPT
}

# 添加指定udp端口防火墙规则
Add_udp_iptables(){
    iptables -I INPUT -m state --state NEW -m udp -p udp --dport ${port} -j ACCEPT
    ip6tables -I INPUT -m state --state NEW -m udp -p udp --dport ${port} -j ACCEPT
}

# 删除指定端口防火墙规则
Del_iptables(){
    iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport ${port} -j ACCEPT   >/dev/null 2>&1
    ip6tables -D INPUT -m state --state NEW -m tcp -p tcp --dport ${port} -j ACCEPT  >/dev/null 2>&1

    iptables -D INPUT -m state --state NEW -m udp -p udp --dport ${port} -j ACCEPT   >/dev/null 2>&1
    ip6tables -D INPUT -m state --state NEW -m udp -p udp --dport ${port} -j ACCEPT  >/dev/null 2>&1
}

# 保存防火墙规则
Save_iptables(){
    if [[ ${release} == "centos" ]]; then
        service iptables save
        service ip6tables save
    else
        iptables-save > /etc/iptables/rules.v4
        ip6tables-save > /etc/iptables/rules.v6
    fi
}

# 设置防火墙规则，下次开机也生效
Set_iptables(){
    if [[ ${release} == "centos" ]]; then
        service iptables save
        service ip6tables save
        chkconfig --level 2345 iptables on
        chkconfig --level 2345 ip6tables on
    else
        iptables-save > /etc/iptables/rules.v4
        ip6tables-save > /etc/iptables/rules.v6
        echo -e '#!/bin/bash\n/sbin/iptables-restore < /etc/iptables/rules.v4\n/sbin/ip6tables-restore < /etc/iptables/rules.v6' > /etc/network/if-pre-up.d/iptables
        chmod +x /etc/network/if-pre-up.d/iptables
    fi
}

# 隐藏的防火墙设置功能菜单  88
hide_menu(){
    echo
    echo -e "${RedBG}   隐藏的高级防火墙设置功能 By 蘭雅sRGB  ${Font}"
    echo -e "${Green}>  1. ss_kcp_speed_udp2raw 端口开放 防火墙规则"
    echo -e ">  2. ss brook 电报代理端口开放 防火墙规则"
    echo -e ">  3. frps_iptables 防火墙规则"
    echo -e ">  4. 菜单项1-2-3全功能开放"
    echo
    read -p "请输入数字(1-4):" num_x
    case "$num_x" in
        1)
        ss_kcp_speed_udp2raw
        ;;
        2)
        ss_bk_tg
        ;;
        3)
        frps_iptables
        ;;
        4)
        ss_kcp_speed_udp2raw
        ss_bk_tg
        frps_iptables
        ;;
        *)
        ;;
        esac
}

# ss_kcp_speed_udp2raw 端口防火墙规则
ss_kcp_speed_udp2raw(){
    # ss+kcp+udp2raw
    iptables -I INPUT -s 127.0.0.1 -p tcp --dport 40000 -j ACCEPT
    iptables -I INPUT -s 127.0.0.1 -p udp --dport 4000 -j ACCEPT
    iptables -I INPUT -p tcp -m tcp --dport 8989 -j DROP

    # speed+udp2raw
    iptables -I INPUT -s 127.0.0.1 -p udp --dport 8888 -j ACCEPT
    iptables -I INPUT -p tcp -m tcp --dport 8899 -j DROP

    Save_iptables

}

# ss brook 电报代理端口开放 防火墙规则
ss_bk_tg(){
    ss_bk_tg=(2018 7731 7979)

    for i in {0..2}
        do
        port=${ss_bk_tg[$i]}
        Add_iptables
    done

    Save_iptables
}

# frps_iptables 防火墙规则
frps_iptables(){
    frps_port=(7000 7500 8080 4443 11122 2222)

    for i in {0..5}
        do
        port=${frps_port[$i]}
        Add_iptables
    done

    Save_iptables
}

# 安全防火墙规则
safe_iptables(){
    iptables -I INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
    iptables -A INPUT -p tcp -m tcp --dport 22  -j ACCEPT
    iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
    iptables -A INPUT -j DROP
    iptables -P FORWARD ACCEPT
    iptables -A OUTPUT -j ACCEPT

    ip6tables -I INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
    ip6tables -A INPUT -p tcp -m tcp --dport 22  -j ACCEPT
#    ip6tables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
    ip6tables -A INPUT -j DROP
    ip6tables -P FORWARD ACCEPT
    ip6tables -A OUTPUT -j ACCEPT

}

# 初始化安全防火墙规则
init_iptables(){
    # 清除防火墙规则
    iptables -F
    ip6tables -F

    # 添加 tcp 端口
    for i in {0..3}
        do
        port=${tcp_port[$i]}
        Add_iptables
    done

    # 添加 udp 端口
    for i in {0..3}
        do
        port=${udp_port[$i]}
        Add_udp_iptables
    done

    safe_iptables
    Set_iptables
}

add_tcp(){
read -p "请输入TCP端口号(20--65000): " port
Add_iptables
safe_iptables
}

add_udp(){
read -p "请输入UDP端口号(20--65000): " port
Add_udp_iptables
safe_iptables
}

del_tcp(){
read -p "请输入TCP端口号(20--65000): " port
Del_iptables
safe_iptables
}

# 禁止ICMP，禁止Ping服务器
no_ping(){
    iptables -I INPUT -p icmp --icmp-type echo-request -j DROP
}

# 设置菜单
start_menu(){
    echo
    echo -e "${RedBG}   IPTABLES 设置防火墙规则 脚本 By 蘭雅sRGB  ${Font}"
    echo -e "${Green}>  1. 添加 TCP 端口到防火墙规则"
    echo -e ">  2. 添加 UDP 端口到防火墙规则"
    echo -e ">  3. 删除TCP/UDP端口防火墙规则"
    echo -e ">  4. 禁止ICMP，禁止Ping服务器"
    echo -e ">  5. 重置 初始化安全防火墙规则预设端口"
    echo -e ">  6. 退出设置${Font}"
    echo
    read -p "请输入数字(1-6):" num
    case "$num" in
        1)
        add_tcp
        ;;
        2)
        add_udp
        ;;
        3)
        del_tcp
        ;;
        4)
        no_ping
        ;;
        5)
        init_iptables
        ;;
        6)
        exit 1
        ;;
        88)
        hide_menu
        ;;
        *)
        echo
        iptables -L -n
        ;;
        esac
}

check_sys
start_menu
