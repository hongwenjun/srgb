## 自己写了个虚拟键盘工具，可以用来VNC时输入密码和长命令

![VNC_KEYS.webp][1]


### 在 DD windows 时 粘贴就用，方便不容易搞错了，登陆时输入长密码也可以使用

![GIF.gif][2]
### 下载源码，建立或修改快捷方式 `虚拟键盘输入`
```
#  C:\Users\vip\AppData\Local\Programs\Python\Python38\pythonw.exe  vitual_keys.py

# 起始位置  %cd%
```
## 源码下载 https://github.com/hongwenjun/srgb/tree/master/vitual_keys
![code.png][3]

```
# 需要先安装 Python 和 pywin32库
# pip install pywin32
import sys
import win32clipboard as w
from keymap import * 

# 剪贴板处理函数 getText setText
def getText():
    w.OpenClipboard()
    d = w.GetClipboardData(win32con.CF_TEXT)
    w.CloseClipboard()
    return(d).decode('gbk')

def setText(aString):
    w.OpenClipboard()
    w.EmptyClipboard()
    w.SetClipboardText(aString)
    w.CloseClipboard()

# 把剪贴板里的文本，虚拟键盘方式单个输入当前窗口
text = getText()
win32api.MessageBox(0, text, "点击确定后，剪贴板内容将在3秒后发送到指定的窗口", win32con.MB_OK | win32con.MB_ICONWARNING)
text = text.replace('\r', '').replace('\t', '    ')
for i in text:
    if ord(i) > 255 :
       win32api.MessageBox(0, '发送文本只能 ASCII 字符', "发送文本只能 ASCII 字符", win32con.MB_OK | win32con.MB_ICONWARNING) 
       sys.exit(1)

#  剪贴板内容将在3秒后发送到指定的窗口 发送文本只能 ASCII 字符
time.sleep(3)
typer(string=text)

```

  [1]: https://262235.xyz/usr/uploads/2021/12/3165532830.webp
  [2]: https://262235.xyz/usr/uploads/2021/12/1931188946.gif
  [3]: https://262235.xyz/usr/uploads/2021/12/2089807561.png
