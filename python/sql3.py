# sqlite3 模块由 Gerhard H?ring 编写。它提供了符合 DB-API 2.0 规范的接口，这个规范是 PEP 249。

# 要使用这个模块，必须先创建一个 Connection 对象，它代表数据库。下面例子中，数据将存储在 example.db 文件中：

import sqlite3
conn = sqlite3.connect('example.db')

# 当有了 Connection 对象后，你可以创建一个 Cursor 游标对象，然后调用它的 execute() 方法来执行 SQL 语句：

c = conn.cursor()

# Create table  建立数据表
c.execute('''CREATE TABLE stocks
             (date text, trans text, symbol text, qty real, price real)''')

# Insert a row of data  插入一行数据
c.execute("INSERT INTO stocks VALUES ('2006-01-05','BUY','RHAT',100,35.14)")

# Save (commit) the changes  保存（提交）更改
conn.commit()

# We can also close the connection if we are done with it. # 如果完成连接，我们也可以关闭连接。
# Just be sure any changes have been committed or they will be lost.  # 只要确保任何的修改已经提交，否则将丢失。
conn.close()


# 这些数据被持久化保存了，而且可以在之后的会话中使用它们：

import sqlite3
conn = sqlite3.connect('example.db')
c = conn.cursor()

'''
通常你的 SQL 操作需要使用一些 Python 变量的值。你不应该使用 Python 的字符串操作来创建你的查询语句，因为那样做不安全；它会使你的程序容易受到 SQL 注入攻击（在 https://xkcd.com/327/ 上有一个搞笑的例子，看看有什么后果）

推荐另外一种方法：使用 DB-API 的参数替换。在你的 SQL 语句中，使用 ? 占位符来代替值，然后把对应的值组成的元组做为 execute() 方法的第二个参数。（其他数据库可能会使用不同的占位符，比如 %s 或者 :1）例如：
'''

# Never do this -- insecure!  # 永远不要这样做-不安全！
symbol = 'RHAT'
c.execute("SELECT * FROM stocks WHERE symbol = '%s'" % symbol)

# Do this instead  # 改为这样操作
t = ('RHAT',)
c.execute('SELECT * FROM stocks WHERE symbol=?', t)
print(c.fetchone())

# Larger example that inserts many records at a time  # 较大的示例一次插入许多记录
purchases = [('2006-03-28', 'BUY', 'IBM', 1000, 45.00),
             ('2006-04-05', 'BUY', 'MSFT', 1000, 72.00),
             ('2006-04-06', 'SELL', 'IBM', 500, 53.00),
            ]
c.executemany('INSERT INTO stocks VALUES (?,?,?,?,?)', purchases)


# 要在执行 SELECT 语句后获取数据，你可以把游标作为 iterator，然后调用它的 fetchone() 
# 方法来获取一条匹配的行，也可以调用 fetchall() 来得到包含多个匹配行的列表。

# 下面是一个使用迭代器形式的例子：

for row in c.execute('SELECT * FROM stocks ORDER BY price'):
	print(row)
