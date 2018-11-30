<pre>

服务器名称	云主机ID	所在网络ID	公网IP
u672au547du540d	ins-97rj9h7x	基础网络	111.231.118.129 

// 安装 编译器  shadowsocks  Apache2 和 php
sudo apt-get update
sudo apt-get install build-essential gcc g++ 
sudo apt-get install shadowsocks-libev
sudo apt-get install Apache2
sudo apt-get install php

// 设置 vim
wget srgb.vicp.net/srgb/vim/_vimrc
mv _vimrc .vimrc

// 修改配置和测试
vim /etc/shadowsocks-libev/config.json

vim /etc/apache2/sites-enabled/000-default.conf

echo  "<?php phpinfo(); ?> " > /var/www/test.php
service apache2 restart

http://111.231.118.129/test.php


vim /etc/rc.local
ss-server -c /etc/shadowsocks-libev/config.json  -f /tmp/ss-server.pid
或者 echo "ss-server -c /etc/shadowsocks-libev/config.json  -f /tmp/ss-server.pid"  >> /etc/rc.local


Shadowsocks 配置
http://127.0.0.1:1080/pac.txt

### 一键脚本 ss_brook 和 个人配置
```
# ss_brook脚本 (私人备份恢复脚本)
wget -qO- git.io/fxQug | bash
```

====测试搬迁服务器==============
tar -czvf srgb.tar.gz index.html CARSCAM.html img script srgb v315  cppprimer  ico reference text vc2012_CRT Trajectory cmdhelp
scp srgb.tar.gz  root@111.231.118.129:/var/www
ssh root@111.231.118.129
tar -xvf srgb.tar.gz


VIM 中可用 :s 命令来替换字符串，具体如下：

:s/str1/str2/ 替换当前行第一个 str1 为 str2 
:s/str1/str2/g 替换当前行中所有 str1 为 str2 
:m,ns/str1/str2/ 替换第 n 行开始到最后一行中每一行的第一个 str1 为 str2 
:m,ns/str1/str2/g 替换第 n 行开始到最后一行中所有的 str1 为 str2 
(注：m和n 为数字，若m为 .，表示为当前行开始；若n为$，则表示到最后一行结束) 
