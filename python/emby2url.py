# python 读写剪切板内容
# pip3 install pywin32

import win32clipboard as w
import win32con
import os

def getText():
    w.OpenClipboard()
    d = w.GetClipboardData(win32con.CF_TEXT)
    w.CloseClipboard()
    return(d).decode('GBK')

def setText(aString):
    w.OpenClipboard()
    w.EmptyClipboard()
    w.SetClipboardText(aString)
    w.CloseClipboard()
    
url = getText()
print(url)

# 替换 EmbyMedia 路径 到URL 给PotPlayer播放视频
emby_path = "/mnt/EmbyMedia/"
http_url = "http://192.168.1.11/"

url = url.replace( emby_path , http_url )
setText(url)
print(url)

print(":: 替换EmbyMedia路径到URL给PotPlayer播放视频，剪贴板已经替换成功！\n")
os.system("pause")