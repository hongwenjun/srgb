'''
注：此脚本为 python3 版
'''

import re
import base64

def selectFile():
    '''利用win32ui选择文件对话框
    如果未安装此模块则弹窗输入路径'''
    
    try:
        import win32ui
        o = win32ui.CreateFileDialog( 1, "", "", 0, "*|*.*|AI|*.ai|EPS|*.eps|INDD|*.indd||")
        o.DoModal()
        return o.GetPathName()
    except Exception:
        path = input('Enter File Path: ')
        return re.sub('^"|"$', '', path)

def extractData(inputFile):
    '''获取XMP信息中的缩略图数据'''

    regex_bool = re.compile(b'<xmpGImg:image>(.*)</xmpGImg:image>')

    with open(inputFile, 'rb') as fobj:
        for line in fobj:
            if regex_bool.search(line):
                # print(line)
                base64str = regex_bool.findall(line)[0].replace(b'&#xA;', b'')
                return base64str

def writePNG(outFile, imgData):
    '''将图像数据保存为png文件'''

    with open(outFile, 'wb') as fobj:
        fobj.write(base64.decodestring(imgData)) 

def main():
    inputFile = selectFile()
    outFile = re.sub(r'(.+)(\.(?:ai|indd|eps))', lambda m: "%s.png" % m.group(1), inputFile, flags=re.IGNORECASE)
    imgData = extractData(inputFile)
    if imgData:
        writePNG(outFile, imgData)
    else:
        print('无缩略图数据')


if __name__ == '__main__':
    main()