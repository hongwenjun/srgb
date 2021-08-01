import urllib
import requests
import re

for i in range(100000):
  try:
    r = requests.head('https://chdbits.co/download.php?id='+ str(i) +'&passkey=1477df8065360758090ba4b431799875')
    d = r.headers['content-disposition']
    fname = re.findall("filename=(.+)", d)[0]
    f = open('result.txt','a')
    f.write('ID:' + str(i) + ' ' + urllib.parse.unquote(fname, encoding='utf-8', errors='replace') + '\n')
    f.close()
    print('ID:' + str(i) + ' ' + urllib.parse.unquote(fname, encoding='utf-8', errors='replace'))
  except:
    print('ID:' + str(i) + '无数据')

