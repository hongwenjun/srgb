#-*- coding: UTF-8 -*- 

import sys , os

# 颜色定义
Green = '\033[32m'
Red = '\033[31m'
GreenBG = '\033[42;37m'
RedBG = '\033[41;37m'
Yellow = '\033[0;33m'
SkyBlue = '\033[0;36m'
Font = '\033[0m'

sys.ps1 = Red + '>' + Yellow + '>' + SkyBlue + '> ' + Font
sys.ps2 = SkyBlue + '... ' + Font

def cls():
    print('\x1bc')

def ls():
    cur_path = os.getcwd()
    os.system('ls -l ' + cur_path)
    print( SkyBlue + os.getcwd(), end = ' ')

def cd(path = '.'):
    os.chdir(path)
    print( SkyBlue + os.getcwd(), end = ' ')

def cat(file = 'me.py'):
    with open(file, 'r') as f:
        print(f.read())

def info():
    print( SkyBlue + ':: Usage:  ' + Green + 'python -i me.py' + Yellow + ' Or, import the module [import me]')
    print( Green   + ':: Function:  me.cls()  me.ls()  me.cd(path)  me.cat(file)  me.info() ')
    print( SkyBlue + os.getcwd(), end = ' ')

info()
