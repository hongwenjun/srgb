# 安装mysql连接库  pip install mysql-connector-python
import mysql.connector

mysql_connection = mysql.connector.connect(
    user = 'root', password = 'baiyue.one', host='192.168.1.133', database ='PythonDB' )

cursor = mysql_connection.cursor()
cursor.execute("CREATE TABLE Students (StudentID int PRIMARY KEY, Name VARCHAR(255), City VARCHAR(255))")