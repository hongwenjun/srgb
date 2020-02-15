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

