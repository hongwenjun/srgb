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

"""

########################################################################################

IPDB 格式官方解析代码
https://www.ipip.net/support/code.html
https://github.com/ipipdotnet/ipdb-python

## qqwry.ipdb   纯真数据库 IPIP.net 格式版，精简并匹配为国家、省、市、运营商。
https://github.com/metowolf/qqwry.ipdb

# 原版  https://cdn.jsdelivr.net/npm/qqwry.raw.ipdb/qqwry.ipdb

#########################################################################################

import ipdb

# db = ipdb.City("/path/to/city.ipv4.ipdb")
db = ipdb.City("qqwry.ipdb")

# db.reload("/path/to/city.ipv4.ipdb") # update ipdb database file reload data
print(db.is_ipv4(), db.is_ipv6())
print(db.languages()) # support language
print(db.fields()) #  support fields
print(db.build_time()) #  build database time
print(db.find("1.1.1.1", "CN")) #  query ip return array
# print(db.find(u"1.1.1.1", "CN")) #  Python 2.7
print(db.find_map("8.8.8.8", "CN")) #  query ip return dict
print(db.find_info("118.28.1.1", "CN").country_name) 
