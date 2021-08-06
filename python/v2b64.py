import requests

url = 'https://v3.v2b64.xyz/c/UUID'
r = requests.get(url)


import base64

s = base64.b64decode(r.text).decode('ascii')
vms = s.split('\n')

print(vms[0])


def print_sever(data):
	print("\033[42;37m:: %s \033[0m" % data['ps'])
	print("UUID: %s\tAdd: %s:%s\n" % (data['id'], data['add'],  data['port']) )

import re
import json

for vmess in vms :
	res = re.match(r"vmess://(.*)", vmess)
	try :
		json_str = base64.b64decode(res[1]).decode('utf-8')
		data = json.loads(json_str)
		print_sever(data)
##		print(data)
	
	except:
		pass

