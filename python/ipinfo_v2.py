# ipinfo.py
import re
import ipdb
import ipaddress

db = ipdb.BaseStation("/app/qqwry.ipdb")

with open("/app/iplist.txt", "r") as f:
    data = f.read()

iplist = re.findall(r"\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b", data)
for ip in iplist:
   try:
      ipaddress.ip_address(ip).is_global
      print(ip, db.find(ip, "CN"))
   except:
      pass

"""
# 使用前先安装 ipip-ipdb 库 和 QQ纯真IP表数据
wget https://cdn.jsdelivr.net/npm/qqwry.ipdb/qqwry.ipdb
apt install python3-pip -y
pip3 install --upgrade pip requests
pip3 install ipip-ipdb
"""
