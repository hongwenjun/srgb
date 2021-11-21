# WordPress和Typecho文章内容批量替换文字的方法
# https://262235.xyz/index.php/archives/702/

import sqlite3
conn = sqlite3.connect('example.db')

# 创建一个 Cursor 游标对象，然后调用它的 execute() 方法来执行 SQL 语句：
c = conn.cursor()

# Typecho SQLite3 数据库替换SQL语句
c.execute("UPDATE typecho_contents SET text = replace(text,'https://262235.xyz','https://us.262235.xyz')")

# Save (commit) the changes  和 关闭数据库连接
conn.commit()
conn.close()


# 查询文章指定类型post，按cid反序，限定5行
for row in c.execute('SELECT * FROM typecho_contents where type="post" ORDER BY cid desc  LIMIT 5'):
    print(row)

# 查询文章，按text序，限定20行
for row in c.execute('SELECT * FROM typecho_contents ORDER BY text LIMIT 20'):
    print(row[0],row[1])
