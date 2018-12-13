```
he.net
域名         srgb18.ga  
ddns code:   Br9LmXp6le1MTSXY

curl -4 "srgb18.ga:Br9LmXp6le1MTSXY@dyn.dns.he.net/nic/update?hostname=srgb18.ga"


https://srgb18.ga:Br9LmXp6le1MTSXY@dyn.dns.he.net/nic/update?hostname=srgb18.ga&myip=35.235.96.85


首先安装curl

apt update && apt install curl

然后

curl -4 "xxxxxxxxxxxxxxxxxxxxxxx:yyyyyyyyyyyyyyyy@dyn.dns.he.net/nic/update?hostname=xxxxxxxxxxxxxxxxxxxxxxx"


xxxxx是你得域名 。 yyyyy为你获得的ddns code

然后设置开启自动运行

crontab -e



```
