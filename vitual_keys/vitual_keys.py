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
