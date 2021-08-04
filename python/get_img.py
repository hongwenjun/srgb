#  Python 网页下载图片学习测试和图片修改
import requests
from bs4 import BeautifulSoup
from urllib.parse import urlsplit
from PIL import Image
from sys import argv

url = 'https://262235.xyz/'
if (len(argv) > 1) :
    url = argv[1]

r = requests.get(url)
# html_doc = r.text

soup = BeautifulSoup(html_doc, 'html5lib')

# print(soup.prettify())

title_tag = soup.title
print(title_tag.text)
print(soup.title.text)

links = soup.find_all('img')
len(links)

url = urlsplit(r.url)
print(url)

for link in links :
	print(link, '\t\t'  ,link['src'])
	
	# 图片链接 支持网址和不带网址
	image_url  = url.scheme + '://' + url.netloc + urlsplit(link['src']).path
	name = image_url.split('/')
	save_image_file = './' + name[-1]
	
	# 图片URL和保存文件名
	print(image_url , save_image_file )
	
	# 下载图片
	r = requests.get(url=image_url)

	# 注意要以二进制只写打开文件
	with open(save_image_file, 'wb') as f:
		# 图片的二进制数据
		f.write(r.content)

	with Image.open(save_image_file) as im:
		im.thumbnail((128,128))
		im.save(save_image_file + ".128.png", "PNG")
