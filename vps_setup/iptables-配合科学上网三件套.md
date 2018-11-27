## iptables 配合科学上网三件套

https://www.lijingquan.net/index.php/2018/06/30/iptables-配合科学上网三件套/
2018年6月30日 BY : TATERLI

### 服务器:

```shell
ss-server -s 127.0.0.1 -p AAAAA -k FFFFF -m aes-256-gcm -t 300 >/dev/null 2>&1 &
kcp-server -t "127.0.0.1:AAAAA" -l "127.0.0.1:BBBBB" --mtu 1200 --mode fast3 --key FFFFF --crypt "none"  >/dev/null 2>&1 &
udp2raw -s -l0.0.0.0:CCCCC -r 127.0.0.1:BBBBB -k FFFFF --raw-mode faketcp --cipher-mode none --auth-mode simple  >/dev/null 2>&1 &
```

### 对应的防火墙规则.
```shell
iptables -F
iptables -I INPUT -s 127.0.0.1 -p tcp --dport AAAAA -j ACCEPT
iptables -I INPUT -s 127.0.0.1 -p udp --dport BBBBB -j ACCEPT
iptables -I INPUT -p tcp -m tcp --dport CCCCC -j DROP
iptables -I INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A INPUT -p tcp -m tcp --dport 22  -j ACCEPT
iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
iptables -A INPUT -j DROP
iptables -P FORWARD ACCEPT
iptables -A OUTPUT -j ACCEPT
iptables-save > /etc/iptables/rules.v4
```

### 客户端:
```shell
udp2raw -c -rxxx.xxx.xxx.xxx:CCCCC -l127.0.0.1:DDDDD -k "FFFFF" --raw-mode faketcp --cipher-mode none --auth-mode simple >/dev/null 2>&1 &
kcptun -l 0.0.0.0:EEEEE -r 127.0.0.1:DDDDD -key "FFFFF" --crypt none --mtu 1200 --mode fast3 >/dev/null 2>&1 &
```

### 对应的防火墙规则.

```shell
iptables -F
iptables -I INPUT -s 127.0.0.1 -p udp --dport DDDDD -j ACCEPT
iptables -I INPUT -s xxx.xxx.xxx.xxx/32 -p tcp -m tcp --sport CCCCC -j DROP
iptables -I INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A INPUT -p tcp -m tcp --dport 22  -j ACCEPT
iptables -A INPUT -p tcp -m tcp --dport EEEEE  -j ACCEPT
iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
iptables -A INPUT -j DROP
iptables -P FORWARD ACCEPT
iptables -A OUTPUT -j ACCEPT
iptables-save > /etc/iptables/rules.v4
```

为什么UDP2RAW要DROP掉呢,据说是因为数据包不是真TCP包,要是全收了,会让内核发出错误的信息,更麻烦.

持久化iptables需要iptables-persistent软件包,可以直接apt安装.
