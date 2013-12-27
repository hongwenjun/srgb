#!/bin/bash

touch /tmp/getwanip.txt

# 移动网络测试正确的
curl -s 192.168.1.1 | grep ipinfo >>/tmp/getwanip.txt              #  如果DD-WRT的路由器这个最方便
curl -s http://www.ip138.com/ips1388.asp | grep ip_add.asp  >>/tmp/getwanip.txt
curl -s http://ns1.dnspod.net:6666    >>/tmp/getwanip.txt
echo "  "  >>/tmp/getwanip.txt
curl -s http://www.net.cn/static/customercare/yourip.asp | grep '<h2>.*</h2>'   >>/tmp/getwanip.txt 

#  移动网络不正确
# curl -s http://myip.dnsdynamic.org/  >>/tmp/getwanip.txt
echo "  "  >>/tmp/getwanip.txt
# curl -s http://ip.3322.net/          >>/tmp/getwanip.txt 
echo "  "  >>/tmp/getwanip.txt
# curl -s -I http://www.alibaba.com | grep ali_apache_id    >>/tmp/getwanip.txt     #  移动网络不正确
#


./getwanip
mv /tmp/getwanip.txt  /tmp/getwanip.old
