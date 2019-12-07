# 文件读取和写入

fout = open('output.txt', 'w')

line1 = "This here's the wattle,\n"
fout.write(line1)

line2 = "the emblem of our land.\n"
fout.write(line2)

# 格式化运算符
x = 52
fout.write(str(x))

camels = 42
fout.write('\nI have spotted %d camels.\n' % camels)
fout.write('In %d years I have spotted %g %s.\n' % (3, 0.1, 'camels'))

fout.close()

# Python 从 try 子句（clause）开始执行。 如果一切正常，那么 except 子句将被跳过。 
# 如果发生异常，则跳出 try 子句，执行 except 子句。
# 使用 try 语句处理异常被称为是 捕获（catching） 异常

try:
    fin = open('output.txt')
    print('output.txt ...OK.\n')
    print(fin.read())
except:
    print('Something went wrong.')

# 文件名和路径
import os
cwd = os.getcwd()      # os.getcwd 返回当前目录的名称
print(cwd) 

 # 从相对路径（relative path）获得一个文件的绝对路径（absolute path）
path = os.path.abspath('output.txt')   
print(path) 

os.path.exists('memo.txt')   # os.path.exists 检查一个文件或者目录是否存在
os.path.isdir('/home/dinsdale')  # 可以通过 os.path.isdir 检查它是否是一个目录

# os.listdir返回给定目录下的文件列表（以及其它目录）
flist = os.listdir(cwd)
print(flist) 

# “遍历”一个目录，打印所有文件的名字，并且针对其中所有的目录递归的调用自身。
# os.path.join接受一个目录和一个文件名，并把它们合并成一个完整的路径。
def walk(dirname):
    for name in os.listdir(dirname):
        path = os.path.join(dirname, name)

        if os.path.isfile(path):
            print(path)
        else:
            walk(path)

print(walk('E:\\python'))

# 数据库
import dbm
db = dbm.open('captions', 'c')
db['cleese.png'] = 'Photo of John Cleese.'
db['silly.png'] = 'Photo of John Cleese doing a silly walk.'

# 字典方法，例如 keys 和 items ，不适用于数据库对象，但是 for 循环依然适用：
for key in db:
    print(key, db[key])

# 当你完成操作后需要关闭文件
db.close()

# Python 中通过使用 管道对象（pipe object） 来启动
filename = 'output.txt'
cmd = 'md5sum ' + filename
fp = os.popen(cmd)
res = fp.read()
print(res)

# 返回值是 ls 进程的最终状态。None 表示正常结束（没有出现错误）
stat = fp.close()
print(stat)

# 编写模块
# 任何包含 Python 代码的文件，都可以作为模块被导入。 例如，假设你有包含以下代码的文件 wc.py ：

def linecount(filename):
    count = 0
    for line in open(filename):
        count += 1
    return count

print(linecount('output.txt'))

