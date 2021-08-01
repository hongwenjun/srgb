#!/usr/bin/python
import requests
import time
import re

# 时间戳
timestamp = str(int(time.time() * 1000))

# 拼接请求地址
url = 'https://cn.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&nc=' + timestamp + '&pid=hp'

# 请求头，模拟浏览器UA
headers = {
    'User-Agent': ' '.join(['Mozilla/5.0 (Windows NT 10.0; Win64; x64; ServiceUI 14)',
                            'AppleWebKit/537.36 (KHTML, like Gecko)', 'Chrome/70.0.3538.102', 'Safari/537.36',
                            'Edge/18.18363'])
}

# 发送请求
r = requests.get(url=url, headers=headers)

# 将响应的字符串转化为json数据，即dict类型
result = r.json()

# 获取第一个图片的链接
image_url = result['images'][0]['url']

# 图片文件名  name[1]
name = re.search( r'id=(.*\.jpg)&rf', image_url)

# 拼接上bing的域名
image_url = 'https://cn.bing.com' + image_url

# 定义图片保存地址
save_image_file = '/home/samba/bing/' + name[1]

# 下载图片
r = requests.get(url=image_url, headers=headers)

# 注意要以二进制只写打开文件
with open(save_image_file, 'wb') as f:
    # 图片的二进制数据
    f.write(r.content)
