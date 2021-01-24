
### mysql 简单使用

![](https://raw.githubusercontent.com/hongwenjun/img/master/python/mysql_cmd.png)
```
# 进入 mysql 容器
docker exec -it root_mysql_1 bash

# 登入 mysql
mysql -u root -p

# 设置显示中文
set names utf8;

# 使用 数据库
USE `PythonDB`;

# 查询 Students 表数据
SELECT * FROM `PythonDB`.`Students` LIMIT 100;

```

### Python 使用 mysql连接库

```
# 安装mysql连接库  pip install mysql-connector-python
import mysql.connector

mysql_connection = mysql.connector.connect(
    user = 'root', password = 'baiyue.one', host='192.168.1.133', database ='PythonDB' )

cursor = mysql_connection.cursor()
cursor.execute('SELECT * FROM Students')


students =  cursor.fetchall()
for student in students:
    print(student)

mysql_connection.close()

```
