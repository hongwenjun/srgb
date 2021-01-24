# 安装mysql连接库  pip install mysql-connector-python
import mysql.connector

mysql_connection = mysql.connector.connect(
    user = 'root', password = 'baiyue.one', host='192.168.1.133' )

cursor = mysql_connection.cursor()
cursor.execute("CREATE DATABASE PythonDB")