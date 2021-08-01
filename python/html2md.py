#!/usr/bin/python3

import requests
import html2text
from sys import argv


# 拼接请求地址
url = 'https://www.262235.xyz/index.php/category/learn/'

if (len(argv) > 1) :
    url = argv[1]

# 请求头，模拟浏览器UA
headers = {
    'User-Agent': ' '.join(['Mozilla/5.0 (Windows NT 10.0; Win64; x64; ServiceUI 14)',
                            'AppleWebKit/537.36 (KHTML, like Gecko)', 'Chrome/70.0.3538.102', 'Safari/537.36',
                            'Edge/18.18363'])
}

# 发送请求
r = requests.get(url=url, headers=headers)

# html 转换 markdown
html = r.text
text = html2text.html2text(html)

# 转换时忽略链接
if (len(argv) > 2) :
    url = argv[1]
    h = html2text.HTML2Text()
    # Ignore converting links from HTML
    h.ignore_links = True
    text = h.handle(html)

print(text)

