import re
import ipdb

db = ipdb.BaseStation("ipipfree.ipdb")

with open("iplist.txt", "r") as f:
    data = f.read()

iplist = re.findall(r"\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b", data)
for ip in iplist:
    print(ip, db.find(ip, "CN"))


"""
# 使用前先安装 ipip-ipdb 库
apt install python3-pip -y
pip3 install --upgrade pip requests
pip3 install ipip-ipdb

# 获得 iplist.txt
bash ~/wgmtu ip > iplist.txt

"""
